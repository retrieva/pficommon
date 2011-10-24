{-# LANGUAGE
  ExistentialQuantification,
  TypeSynonymInstances,
  FlexibleContexts,
  UndecidableInstances
 #-}

module PFIRPC(
  RPCClient(..),
  RPCType(..),
  RPCValue(..),
  BinaryRPCClient(..),

  call,
) where

import Control.Monad

import qualified Data.ByteString as BS
import Data.ByteString (ByteString)
import Data.Bits
import qualified Data.Map as Map
import Data.Map (Map)

import Foreign.C
import Foreign.Marshal.Array
import Foreign.Marshal.Utils
import Foreign.Ptr
import Foreign.Storable

import Network
import System.IO
import System.IO.Unsafe

-----

-- Connector class

class RPCClient c where
  connect :: String -> Int -> IO c
  send    :: c -> ByteString -> IO ()
  recv    :: c -> Int -> IO ByteString
  flush   :: c -> IO ()

-- Value class

class RPCType r where
  sendValue :: RPCClient c => c -> r -> IO ()
  recvValue :: RPCClient c => c -> IO r

data RPCValue = forall r. (RPCType r) => RPCValue r

sendRPCValue c (RPCValue v) = sendValue c v

-- Serializer

class BSSerialize d where
  toBS   :: d -> ByteString
  fromBS :: ByteString -> d

instance BSSerialize Char where
  toBS n = BS.pack [ fromIntegral (fromEnum n .&. 0xff) ]
  fromBS bs = let [a] = map fromIntegral $ BS.unpack bs in toEnum a

instance BSSerialize Int where
  toBS n = BS.pack $ map fromIntegral [ n.&.0xff
                                      , n`shiftR`8 .&.0xff
                                      , n`shiftR`16.&.0xff
                                      , n`shiftR`24.&.0xff ]
  fromBS bs =
    let [a, b, c, d] = map fromIntegral $ BS.unpack bs in
    a .|. (b`shiftL`8) .|. (c`shiftL`16) .|. (d`shiftL`24)

instance BSSerialize Double where
  toBS d = unsafePerformIO $ do
    let cd = realToFrac d :: CDouble
    with cd $ \p -> do
      let b8p = castPtr p
      ws <- peekArray 8 b8p
      return $ BS.pack ws

  fromBS bs = unsafePerformIO $ do
    let ws = BS.unpack bs
    withArray ws $ \w8p -> do
      let cdp = castPtr w8p
      cd <- peek cdp
      return $ realToFrac (cd :: CDouble)

instance RPCType Char where
  sendValue c n = send c (toBS n)
  recvValue c   = recv c 1 >>= (return . fromBS)

instance RPCType Int where
  sendValue c n = send c (toBS n)
  recvValue c   = recv c 4 >>= (return . fromBS)

instance RPCType Double where
  sendValue c n = send c (toBS n)
  recvValue c   = recv c 8 >>= (return . fromBS)

instance RPCType a => RPCType [a] where
  sendValue c xs = do
    send c (toBS $ length xs)
    mapM_ (sendValue c) xs
  recvValue c = do
    n  <- recvValue c
    replicateM n (recvValue c)

{-
instance RPCType String where
  sendValue c s = do
    send c (toBS $ length s)
    send c (BS.pack $ map (fromIntegral.fromEnum) s)
  recvValue c = do
    n  <- recvValue c
    bs <- recv c n
    return $ map (toEnum.fromIntegral) $ BS.unpack bs
-}

instance (Ord k, RPCType k, RPCType v) => RPCType (Map k v) where
  sendValue c xs = do
    send c (toBS $ Map.size xs)
    mapM_ (\(k, v) -> sendValue c k >> sendValue c v) $ Map.toList xs
  recvValue c = do
    n  <- recvValue c
    xs <- replicateM n (recvValue c >>= \k -> recvValue c >>= \v -> return (k, v))
    return $ Map.fromList xs

pingPong c = do
  sendValue c "<<<PING>>>"
  flush c
  pong <- recvValue c
  when (pong/="<<<PONG>>>") $ error "connection establish failed"

checkCode c = do
  code <- recvValue c
  when (code/="OK") $ error ("wrong result code: " ++ code)

call :: (RPCType r, RPCClient c) => c -> String -> Int -> [RPCValue] -> IO r
call c method version vals = do
  pingPong c
  sendValue c method
  sendValue c version
  mapM_ (sendRPCValue c) vals
  flush c
  checkCode c
  recvValue c

data BinaryRPCClient = BinaryRPCClient
  { handle   :: Handle }

instance RPCClient BinaryRPCClient where
  connect host port = do
    h <- connectTo host (PortNumber $ fromIntegral port)
    return $ BinaryRPCClient { handle = h }

  send  (BinaryRPCClient h) bs = BS.hPut h bs
  recv  (BinaryRPCClient h) n  = BS.hGet h n
  flush (BinaryRPCClient h)    = hFlush h
