// Copyright (c)2008-2011, Preferred Infrastructure Inc.
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
// 
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
// 
//     * Neither the name of Preferred Infrastructure nor the names of other
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef INCLUDE_GUARD_PFI_NETWORK_CGI_TAGS_H_
#define INCLUDE_GUARD_PFI_NETWORK_CGI_TAGS_H_

#include "inserter.h"

#define tag__(name) \
  if (pfi::network::cgi::tag_inserter __TAG__=pfi::network::cgi::tag_inserter(name, builder)) {} else

#define attr__(name) \
  __TAG__.attr(name)

#define text__(s) \
  pfi::network::cgi::text_inserter(s, builder)

#define prim__(s) \
  pfi::network::cgi::prim_inserter(s, builder)

#define a__       tag__("a")
#define abbr__    tag__("abbr")
#define acronym__ tag__("acronym")
#define address__ tag__("address")
#define applet__  tag__("applet")
#define area__    tag__("area")

#define b__          tag__("b")
#define base__       tag__("base")
#define basefont__   tag__("basefont")
#define bdo__        tag__("bdo")
#define bgsound__    tag__("bgsound")
#define big__        tag__("big")
#define blink__      tag__("blink")
#define blockquote__ tag__("blockquote")
#define body__       tag__("body")
#define br__         tag__("br")
#define button__     tag__("button")

#define caption__  tag__("caption")
#define center__   tag__("center")
#define cite__     tag__("cite")
#define code__     tag__("code")
#define col__      tag__("col")
#define colgroup__ tag__("colgroup")
#define comment__  tag__("comment")

#define dd__  tag__("dd")
#define del__ tag__("del")
#define dfn__ tag__("dfn")
#define dir__ tag__("dir")
#define div__ tag__("div")
#define dl__  tag__("dl")
#define dt__  tag__("dt")

#define em__    tag__("em")
#define embed__ tag__("embed")

#define fieldset__ tag__("fieldset")
#define font__     tag__("font")
#define form__     tag__("form")
#define frame__    tag__("frame")
#define frameset__ tag__("frameset")

#define h1__   tag__("h1")
#define h2__   tag__("h2")
#define h3__   tag__("h3")
#define h4__   tag__("h4")
#define h5__   tag__("h5")
#define h6__   tag__("h6")
#define head__ tag__("head")
#define hr__   tag__("hr")
#define html__ tag__("html")

#define i__       tag__("i")
#define iframe__  tag__("iframe")
#define ilayer__  tag__("ilayer")
#define img__     tag__("img")
#define input__   tag__("input")
#define ins__     tag__("ins")
#define isindex__ tag__("isindex")

#define kbd__ tag__("kbd")

#define label__   tag__("label")
#define layer__   tag__("layer")
#define legend__  tag__("legend")
#define li__      tag__("li")
#define link__    tag__("link")
#define listing__ tag__("listing")

#define map__      tag__("map")
#define marquee__  tag__("marquee")
#define menu__     tag__("menu")
#define meta__     tag__("meta")
#define multicol__ tag__("multicol")

#define nobr__     tag__("nobr")
#define noembed__  tag__("noembed")
#define noframes__ tag__("noframes")
#define nolayer__  tag__("nolayer")
#define noscript__ tag__("noscript")

#define object__   tag__("object")
#define ol__       tag__("ol")
#define optgroup__ tag__("optgroup")
#define option__   tag__("option")

#define p__         tag__("p")
#define param__     tag__("param")
#define plaintext__ tag__("plaintext")
#define pre__       tag__("pre")

#define q__ tag__("q")

#define rb__   tag__("rb")
#define rp__   tag__("rp")
#define rt__   tag__("rt")
#define ruby__ tag__("ruby")

#define s__      tag__("s")
#define samp__   tag__("samp")
#define script__ tag__("script")
#define select__ tag__("select")
#define small__  tag__("small")
#define spacer__ tag__("spacer")
#define span__   tag__("span")
#define strike__ tag__("strike")
#define strong__ tag__("strong")
#define style__  tag__("style")
#define sub__    tag__("sub")
#define sup__    tag__("sup")

#define table__    tag__("table")
#define tbody__    tag__("tbody")
#define td__       tag__("td")
#define textarea__ tag__("textarea")
#define tfoot__    tag__("tfoot")
#define th__       tag__("th")
#define thead__    tag__("thead")
#define title__    tag__("title")
#define tr__       tag__("tr")
#define tt__       tag__("tt")

#define u__  tag__("u")
#define ul__ tag__("ul")

#define var__ tag__("var")

#define wbr__ tag__("wbr")

#define xmp__ tag__("xmp")

#define abbr_a__         attr__("abbr")
#define accesskey__      attr__("accesskey")
#define accept__         attr__("accept")
#define accept_charset__ attr__("accept-charset")
#define action__         attr__("action")
#define align__          attr__("align")
#define alink__          attr__("alink")
#define alt__            attr__("alt")
#define archive__        attr__("archive")
#define axis__           attr__("axis")

#define background__ attr__("background")
#define bgcolor__    attr__("bgcolor")
#define border__     attr__("border")

#define cellpadding__ attr__("cellpadding")
#define cellspacing__ attr__("cellspacing")
#define checked__     attr__("checked")
#define char__        attr__("char")
#define charoff__     attr__("charoff")
#define charset__     attr__("charset")
#define cite_a__      attr__("cite")
#define class__       attr__("class")
#define classid__     attr__("classid")
#define clear__       attr__("clear")
#define code_a__       attr__("code")
#define codetype__    attr__("codetype")
#define codebase__    attr__("codebase")
#define color__       attr__("color")
#define cols__        attr__("cols")
#define colspan__     attr__("colspan")
#define compact__     attr__("compact")
#define content__     attr__("content")
#define coords__      attr__("coords")

#define data__     attr__("data")
#define declare__  attr__("declare")
#define datatime__ attr__("datatime")
#define defer__    attr__("defer")
#define dir_a__    attr__("dir")
#define disabled__ attr__("disabled")

#define enctype__ attr__("enctype")

#define face__        attr__("face")
#define for__         attr__("for")
#define frame_a__     attr__("frame")
#define frameborder__ attr__("frameborder")

#define headers__    attr__("headers")
#define height__     attr__("height")
#define href__       attr__("href")
#define hreflang__   attr__("hreflang")
#define hspace__     attr__("hspace")
#define http_equiv__ attr__("http-equiv")

#define id__    attr__("id")
#define ismap__ attr__("ismap")

#define label_a__  attr__("label")
#define lang__     attr__("lang")
#define language__ attr__("language")
#define link_a__     attr__("link")
#define longdesc__ attr__("longdesc")

#define marginheight__ attr__("marginheight")
#define marginwidth__  attr__("marginwidth")
#define maxlength__    attr__("maxlength")
#define media__        attr__("media")
#define method__       attr__("method")
#define multiple__     attr__("multiple")

#define name__     attr__("name")
#define nohref__   attr__("nohref")
#define noresize__ attr__("noresize")
#define noshade__  attr__("noshade")
#define nowrap__   attr__("nowrap")

#define object_a__ attr__("object")

#define readonly__ attr__("readonly")
#define rel__      attr__("rel")
#define rev__      attr__("rev")
#define rows__     attr__("rows")
#define rowspan__  attr__("rowspan")
#define rules__    attr__("rules")

#define scrolling__ attr__("scrolling")
#define scope__     attr__("scope")
#define selected__  attr__("selected")
#define shape__     attr__("shape")
#define size__      attr__("size")
#define span_a__    attr__("span")
#define src__       attr__("src")
#define start__     attr__("start")
#define standby__   attr__("standby")
#define style_a__   attr__("style")
#define summary__   attr__("summary")

#define tabindex__ attr__("tabindex")
#define target__   attr__("target")
#define text_a__   attr__("text")
#define title_a__  attr__("title")
#define type__     attr__("type")

#define usemap__ attr__("usemap")

#define valign__    attr__("valign")
#define value__     attr__("value")
#define valuetype__ attr__("valuetype")
#define vlink__     attr__("vlink")
#define vspace__    attr__("vspace")

#define width__ attr__("width")

#define onLoad__      attr__("onLoad")
#define onUnload__    attr__("onUnload")
#define onClick__     attr__("onClick")
#define onDblClick__  attr__("onDblClick")
#define onMouseDown__ attr__("onMouseDown")
#define onMouseUp__   attr__("onMouseUp")
#define onMouseOver__ attr__("onMouseOver")
#define onMouseOut__  attr__("onMouseOut")
#define onMouseMove__ attr__("onMouseMove")
#define onKeyPress__  attr__("onKeyPress")
#endif // #ifndef INCLUDE_GUARD_PFI_NETWORK_CGI_TAGS_H_
