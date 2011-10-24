<?
class buffer{
  var $f;
  var $buf;

  public function __construct($f){
    $this->f=$f;
    //stream_set_write_buffer($this->f, 8*1024);
    $this->buf="";
  }

  public function read_u32(){
    $data=$this->read(4);
    $arr=unpack('V', $data);
    return $arr[1];
  }

  public function read_s32(){
    $v=$this->read_u32();
    if ($v&0x80000000){
      $v-=0xffffffff;
      $v--;
    }
    return $v;
  }

  public function read_u16(){
    $data=$this->read(2);
    $arr=unpack('v', $data);
    return $arr[1];
  }

  public function read_s16(){
    $v=$this->read_u16();
    if ($v&0x8000) $v-=0x10000;
    return $v;
  }

  public function read_u8(){
    $data=$this->read(1);
    $arr=unpack('C', $data);
    return $arr[1];
  }

  public function read_s8(){
    $data=$this->read(1);
    $arr=unpack('c', $data);
    return $arr[1];
  }

  public function read_double(){
    $data=$this->read(8);
    $arr=unpack('d', $data);
    return $arr[1];
  }

  public function write_u32($v){
    $data=pack('V', $v);
    $this->write($data, 4);
  }

  public function write_s32($v){
    $this->write_u32($v);
  }

  public function write_u16($v){
    $data=pack('v', $v);
    $this->write($data, 2);
  }

  public function write_s16($v){
    $this->write_u16($v);
  }

  public function write_u8($v){
    $data=pack('C', $v);
    $this->write($data, 1);
  }

  public function write_s8($v){
    $this->write_u8($v);
  }

  public function write_double($v){
    $data=pack('d', $v);
    $this->write($data, 8);
  }

  public function flush(){
    fwrite($this->f, $this->buf, strlen($this->buf));
    $this->buf="";
  }

  public function read($n){
    return fread($this->f, $n);
  }

  public function write($str, $n){
    $this->buf.=substr($str, 0, $n);
    //fwrite($this->f, $str, $n);
  }
};

class int_type{
  var $sign;
  var $size;

  function __construct($sign=false, $size=4){
    $this->sign=$sign;
    $this->size=$size;
  }

  function read($buf){
    if ($this->sign){
      if ($this->size==1) return $buf->read_s8();
      if ($this->size==2) return $buf->read_s16();
      if ($this->size==4) return $buf->read_s32();
    }
    else{
      if ($this->size==1) return $buf->read_u8();
      if ($this->size==2) return $buf->read_u16();
      if ($this->size==4) return $buf->read_u32();
    }
  }

  function write($buf, $v){
    if ($this->sign){
      if ($this->size==1) $buf->write_s8($v);
      if ($this->size==2) $buf->write_s16($v);
      if ($this->size==4) $buf->write_s32($v);
    }
    else{
      if ($this->size==1) $buf->write_u8($v);
      if ($this->size==2) $buf->write_u16($v);
      if ($this->size==4) $buf->write_u32($v);
    }
  }
}

class string_type{
  function read($buf){
    $len=$buf->read_u32();
    return $buf->read($len);
  }

  function write($buf, $str){
    $len=strlen($str);
    $buf->write_u32($len);
    $buf->write($str, $len);
  }
}

class float_type{
  function read($buf){
    return $buf->read_double();
  }
  function write($buf, $v){
    $buf->write_double($v);
  }
}

class array_type{
  var $elemtype;

  function __construct($ty){
    $this->elemtype=$ty;
  }

  function read($buf){
    $size=$buf->read_u32();
    $ret=array();
    for ($i=0; $i<$size; $i++)
      $ret[]=$this->elemtype->read($buf);
    return $ret;
  }

  function write($buf, $arr){
    $size=count($arr);
    $buf->write_u32($size);
    for ($i=0; $i<$size; $i++)
      $this->elemtype->write($buf, $arr[$i]);
  }
}

class map_type{
  var $key_type;
  var $value_type;

  function __construct($kt, $vt){
    $this->key_type=$kt;
    $this->value_type=$vt;
  }

  function read($buf){
    $size=$buf->read_u32();
    $ret=array();
    for ($i=0; $i<$size; $i++){
      $k=$this->key_type->read($buf);
      $v=$this->value_type->read($buf);
      $ret[$k]=$v;
    }
    return $ret;
  }

  function write($buf, $mp){
    $size=count($mp);
    $buf->write_u32($size);
    foreach ($mp as $k => $v){
      $this->key_type->write($buf, $k);
      $this->value_type->write($buf, $v);
    }
  }
};

class class_type{
  var $name;
  var $elems;

  function __construct($name, $tys){
    $this->name=$name;
    $this->elems=$tys;
  }

  function read($buf){
    $ret=new $this->name();
    foreach($this->elems as $memb => $type){
      $ret->$memb=$type->read($buf);
    }
    return $ret;
  }

  function write($buf, $v){
    foreach($this->elems as $memb => $type){
      $type->write($buf, $v->$memb);
    }
  }
}
?>
