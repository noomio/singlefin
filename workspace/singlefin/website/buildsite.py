#!/usr/bin/env python2
#
#  Build Duktape website.  Must be run with cwd in the website/ directory.
#

import os
import sys
import traceback
import time
import datetime
import shutil
import re
import tempfile
import atexit
import md5
import json
import yaml
from bs4 import BeautifulSoup, Tag

colorize = True
fancy_stack = True
remove_fixme = True
testcase_refs = False
list_tags = False
floating_list_tags = True
fancy_releaselog = True

dt_now = datetime.datetime.utcnow()

def readFile(x):
    f = open(x, 'rb')
    data = f.read()
    f.close()
    return data

def htmlEscape(x):
    res = ''
    esc = '&<>'
    for c in x:
        if ord(c) >= 0x20 and ord(c) <= 0x7e and c not in esc:
            res += c
        else:
            res += '&#x%04x;' % ord(c)
    return res

def getAutodeleteTempname():
    tmp = tempfile.mktemp(suffix='duktape-website')
    def f():
        os.remove(tmp)
    atexit.register(f)
    return tmp

# also escapes text automatically
def sourceHighlight(x, sourceLang):
    tmp1 = getAutodeleteTempname()
    tmp2 = getAutodeleteTempname()
    tmpl = r"C:/Program Files (x86)/GnuWin32/share/source-highlight"
    

    f = open(tmp1, 'wb')  # FIXME
    f.write(x)
    f.close()


    # FIXME: safer execution
    os.system('source-highlight --data-dir "%s" -s %s -c highlight.css --no-doc <"%s" >"%s"' % \
              (tmpl,sourceLang, tmp1, tmp2))

    f = open(tmp2, 'rb')
    res = f.read()
    f.close()

    return res

def rst2Html(filename):
    tmp1 = getAutodeleteTempname()

    # FIXME: safer execution
    os.system('python C:/Users/nikol/Anaconda2/Scripts/rst2html.py "%s" >"%s"' % \
              (filename, tmp1))

    f = open(tmp1, 'rb')
    res = f.read()
    f.close()

    return res

def getFileMd5(filename):
    if not os.path.exists(filename):
        return None
    f = open(filename, 'rb')
    d = f.read()
    f.close()
    return md5.md5(d).digest().encode('hex')

def stripNewline(x):
    if len(x) > 0 and x[-1] == '\n':
        return x[:-1]
    return x

def splitNewlineNoLastEmpty(x):
    assert(x is not None)
    res = x.split('\n')
    if len(res) > 0 and res[-1] == '':
        res = res[:-1]
    return res

def validateAndParseHtml(data):
    # first parse as xml to get errors out
    ign_soup = BeautifulSoup(data, 'xml')

    # then parse as lenient html, no xml tags etc
    soup = BeautifulSoup(data, 'lxml')

    return soup

re_stack_line = re.compile(r'^(\[[^\x5d]+\])(?:\s+->\s+(\[[^\x5d]+\]))?(?:\s+(.*?))?\s*$')
def renderFancyStack(inp_line):
    # Support various notations here:
    #
    #   [ a b c ]
    #   [ a b c ] -> [ d e f ]
    #   [ a b c ] -> [ d e f ]  (if foo)
    #

    m = re_stack_line.match(inp_line)
    #print(inp_line)
    assert(m is not None)
    stacks = [ m.group(1) ]
    if m.group(2) is not None:
        stacks.append(m.group(2))

    res = []

    res.append('<div class="stack-wrapper">')
    for idx, stk in enumerate(stacks):
        if idx > 0:
            res.append('<span class="arrow"><b>&rarr;</b></span>')
        res.append('<span class="stack">')
        for part in stk.split(' '):
            part = part.strip()
            elem_classes = []
            elem_classes.append('elem')  #FIXME
            if len(part) > 0 and part[-1] == '!':
                part = part[:-1]
                elem_classes.append('active')
            elif len(part) > 0 and part[-1] == '*':
                part = part[:-1]
                elem_classes.append('referred')
            elif len(part) > 0 and part[-1] == '?':
                part = part[:-1]
                elem_classes.append('ghost')

            text = part

            # FIXME: detect special constants like "true", "null", etc?
            if text in [ 'undefined', 'null', 'true', 'false', 'NaN' ] or \
               (len(text) > 0 and text[0] == '"' and text[-1] == '"'):
                elem_classes.append('literal')

            # FIXME: inline elements for reduced size?
            # The stack elements use a classless markup to minimize result
            # HTML size.  HTML inline elements are used to denote different
            # kinds of elements; the elements should be reasonable for text
            # browsers so a limited set can be used.
            use_inline = False

            if part == '':
                continue
            if part == '[':
                #res.append('<em>[</em>')
                res.append('<span class="cap">[</span>')
                continue
            if part == ']':
                #res.append('<em>]</em>')
                res.append('<span class="cap">]</span>')
                continue

            if part == '...':
                text = '. . .'
                elem_classes.append('ellipsis')
            else:
                text = part

            if 'ellipsis' in elem_classes and use_inline:
                res.append('<i>' + htmlEscape(text) + '</i>')
            elif 'active' in elem_classes and use_inline:
                res.append('<b>' + htmlEscape(text) + '</b>')
            else:
                res.append('<span class="' + ' '.join(elem_classes) + '">' + htmlEscape(text) + '</span>')

        res.append('</span>')

    # FIXME: pretty badly styled now
    if m.group(3) is not None:
        res.append('<span class="stack-comment">' + htmlEscape(m.group(3)) + '</span>')

    res.append('</div>')

    return ' '.join(res) + '\n'  # stack is a one-liner; spaces are for text browser rendering

# C99: these are used if available
type_repl_c99_32bit = [
    ['duk_int_t', 'int' ],
    ['duk_uint_t', 'unsigned int' ],
    ['duk_int32_t', 'int32_t' ],
    ['duk_uint32_t', 'uint32_t' ],
    ['duk_uint16_t', 'uint16_t' ],
    ['duk_idx_t', 'int' ],
    ['duk_uarridx_t', 'unsigned int' ],
    ['duk_codepoint_t', 'int' ],
    ['duk_errcode_t', 'int' ],
    ['duk_bool_t', 'int' ],
    ['duk_ret_t', 'int' ],
    ['duk_size_t', 'size_t' ],
    ['duk_double_t', 'double' ],
]

# Typical 32-bit legacy/embedded platform (32-bit int)
type_repl_legacy32 = [
    ['duk_int_t', 'int' ],
    ['duk_uint_t', 'unsigned int' ],
    ['duk_int32_t', 'int' ],
    ['duk_uint32_t', 'unsigned int' ],
    ['duk_uint16_t', 'unsigned short' ],
    ['duk_idx_t', 'int' ],
    ['duk_uarridx_t', 'unsigned int' ],
    ['duk_codepoint_t', 'int' ],
    ['duk_errcode_t', 'int' ],
    ['duk_bool_t', 'int' ],
    ['duk_ret_t', 'int' ],
    ['duk_size_t', 'size_t' ],
    ['duk_double_t', 'double' ],
]

# Typical 16-bit legacy/embedded platform (16-bit int/short, 32-bit long)
type_repl_legacy16 = [
    ['duk_int_t', 'long' ],
    ['duk_uint_t', 'unsigned long' ],
    ['duk_int32_t', 'long' ],
    ['duk_uint32_t', 'unsigned long' ],
    ['duk_uint16_t', 'unsigned short' ],
    ['duk_idx_t', 'long' ],
    ['duk_uarridx_t', 'unsigned long' ],
    ['duk_codepoint_t', 'long' ],
    ['duk_errcode_t', 'long' ],
    ['duk_bool_t', 'int' ],
    ['duk_ret_t', 'int' ],
    ['duk_size_t', 'size_t' ],
    ['duk_double_t', 'double' ],
]

def substitutePrototypeTypes(line, repl):
    # Replace Duktape custom wrapped types with more concrete counterparts

    line = unicode(line)
    for t in repl:
        line = line.replace(t[0], t[1])
    return line

def processApiDoc(doc, testrefs, used_tags):
    res = []

    # the 'hidechar' span is to allow browser search without showing the char
    res.append('<h1 id="%s" class="apih1">' % doc['name'])
    res.append('<a href="#%s"><span class="hidechar">.</span>%s</a>' % (doc['name'], doc['name']))
    if floating_list_tags and len(doc['tags']) > 0:
        # Sort, reverse order because tags are floated to right
        # (visual order is reverse of DOM order).  Sort version
        # number tags last.

        def mycmp(a,b):
            return cmp( (a[0].isdigit(), a), (b[0].isdigit(), b) )
        p = sorted(doc['tags'], reverse=True, cmp=mycmp)

        # 'introduced' is automatically added as a tag now
        #if doc.has_key('introduced'):
        #    p = [ doc['introduced'] ] + p
        if doc.has_key('deprecated'):
            # XXX: must mark deprecation
            pass
        if doc.has_key('removed'):
            # XXX: must mark removal
            pass

        for idx, val in enumerate(p):
            classes = [ 'apitag' ]
            if val == 'experimental' or val == 'nonportable':
                classes.append('apitagwarn')
            if val == 'protected':
                classes.append('apitagprotected')
            res.append('<a class="' + ' '.join(classes) + '" ' +
                       'href="#' + htmlEscape('taglist-' + val) + '">' + htmlEscape(val) + '</a>')
    res.append('</h1>')

    res.append('<div class="api-call">')

    if doc.has_key('proto'):
        p = splitNewlineNoLastEmpty(doc['proto'])
        res.append('<div class="api-part">')
        res.append('<h2 class="api-proto">Prototype</h2>')
        alt_typing_c99 = []
        alt_typing_legacy32 = []
        alt_typing_legacy16 = []
        for i in p:
            alt_typing_c99.append(substitutePrototypeTypes(i, type_repl_c99_32bit))
            alt_typing_legacy32.append(substitutePrototypeTypes(i, type_repl_legacy32))
            alt_typing_legacy16.append(substitutePrototypeTypes(i, type_repl_legacy16))
        # Long tooltips are a bad idea in most browsers, so just put the C99 typing there for now
        #res.append('<pre class="c-code" title="' +
        #           'C99/C++11: ' + '\n'.join(alt_typing_c99) + '\n' +
        #           'Legacy 32-bit: ' + '\n'.join(alt_typing_legacy32) + '\n' +
        #           'Legacy 16-bit: ' + '\n'.join(alt_typing_legacy16) + '\n'
        #           '">')
        res.append('<pre class="c-code" title="' +
                   'C99/C++11 (32-bit): ' + '\n'.join(alt_typing_c99) +
                   '">')
        for i in p:
            res.append(htmlEscape(i))
        res.append('</pre>')
        res.append('</div>')  # api-part
        res.append('')
    else:
        pass


    if doc.has_key('proto-javascript'):
        p = splitNewlineNoLastEmpty(doc['proto-javascript'])
        res.append('<div class="api-part">')
        res.append('<h2 class="api-proto">Prototype</h2>')

        # Long tooltips are a bad idea in most browsers, so just put the C99 typing there for now
        #res.append('<pre class="c-code" title="' +
        #           'C99/C++11: ' + '\n'.join(alt_typing_c99) + '\n' +
        #           'Legacy 32-bit: ' + '\n'.join(alt_typing_legacy32) + '\n' +
        #           'Legacy 16-bit: ' + '\n'.join(alt_typing_legacy16) + '\n'
        #           '">')
        res.append('<pre class="ecmascript-code">')
        for i in p:
            res.append(htmlEscape(i))
        res.append('</pre>')
        res.append('</div>')  # api-part
        res.append('')
    else:
        pass

    if doc.has_key('macros'):
        p = splitNewlineNoLastEmpty(doc['macros'])
        res.append('<div class="api-part">')
        res.append('<h2 class="api-proto">Macros</h2>')
        alt_typing_c99 = []
        alt_typing_legacy32 = []
        alt_typing_legacy16 = []
        for i in p:
            alt_typing_c99.append(substitutePrototypeTypes(i, type_repl_c99_32bit))
            alt_typing_legacy32.append(substitutePrototypeTypes(i, type_repl_legacy32))
            alt_typing_legacy16.append(substitutePrototypeTypes(i, type_repl_legacy16))
        # Long tooltips are a bad idea in most browsers, so just put the C99 typing there for now
        #res.append('<pre class="c-code" title="' +
        #           'C99/C++11: ' + '\n'.join(alt_typing_c99) + '\n' +
        #           'Legacy 32-bit: ' + '\n'.join(alt_typing_legacy32) + '\n' +
        #           'Legacy 16-bit: ' + '\n'.join(alt_typing_legacy16) + '\n'
        #           '">')
        res.append('<pre class="c-code" title="' +
                   'C99/C++11 (32-bit): ' + '\n'.join(alt_typing_c99) +
                   '">')
        for i in p:
            res.append(htmlEscape(i))
        res.append('</pre>')
        res.append('</div>')  # api-part
        res.append('')
    else:
        pass

    if doc.has_key('callbacks'):
        p = splitNewlineNoLastEmpty(doc['callbacks'])
        res.append('<div class="api-part">')
        res.append('<h2 class="api-proto">Callbacks</h2>')
        res.append('<pre class="c-code">')
        for i in p:
            res.append(htmlEscape(i))
        res.append('</pre>')
        res.append('</div>')  # api-part
        res.append('')
    else:
        pass

    if doc.has_key('functions'):
        p = splitNewlineNoLastEmpty(doc['functions'])
        res.append('<div class="api-part">')
        res.append('<h2 class="api-proto">Functions</h2>')
        res.append('<pre class="c-code">')
        for i in p:
            res.append(htmlEscape(i))
        res.append('</pre>')
        res.append('</div>')  # api-part
        res.append('')
    else:
        pass

    if doc.has_key('details'):
        p = splitNewlineNoLastEmpty(doc['details'])
        res.append('<div class="api-part">')
        res.append('<h2 class="api-proto">Function Details</h2>')
        res.append('<pre class="c-code">')
        for i in p:
            res.append(htmlEscape(i))
        res.append('</pre>')
        res.append('</div>')  # api-part
        res.append('')
    else:
        pass

    if doc.has_key('defines'):
        p = splitNewlineNoLastEmpty(doc['defines'])
        res.append('<div class="api-part">')
        res.append('<h2 class="api-proto">Defines</h2>')
        res.append('<pre class="c-code">')
        for i in p:
            res.append(htmlEscape(i))
        res.append('</pre>')
        res.append('</div>')  # api-part
        res.append('')
    else:
        pass

    if doc.has_key('stack'):
        p = splitNewlineNoLastEmpty(doc['stack'])
        res.append('<div class="api-part">')
        res.append('<h2 class="api-stack">Stack</h2>')
        assert(len(p) > 0)
        for line in p:
            res.append('<pre class="stack">' + \
                       '%s' % htmlEscape(line) + \
                       '</pre>')
        res.append('</div>')
        res.append('')
    else:
        pass
        #res.append('<div class="api-part">')
        #res.append('<h2 class="api-stack">Stack</h2>')
        #res.append('<p>(No effect on value stack.)</p>')
        #res.append('</div>')  # api-part
        #res.append('')

    if doc.has_key('summary'):
        p = splitNewlineNoLastEmpty(doc['summary'])
        res.append('<div class="api-part">')
        res.append('<h2 class="api-summary">Summary</h2>')

        # If text contains a '<p>', assume it is raw HTML; otherwise
        # assume it is a single paragraph (with no markup) and generate
        # paragraph tags, escaping into HTML

        raw_html = False
        for i in p:
            if '<p>' in i:
                raw_html = True

        if raw_html:
            for i in p:
                res.append(i)
        else:
            res.append('<p>')
            for i in p:
                res.append(htmlEscape(i))
            res.append('</p>')
        res.append('</div>')  # api-part
        res.append('')

    if doc.has_key('example'):
        p = splitNewlineNoLastEmpty(doc['example'])
        res.append('<div class="api-part">')
        res.append('<h2 class="api-example">Example</h2>')
        res.append('<pre class="c-code">')
        for i in p:
            res.append(htmlEscape(i))
        res.append('</pre>')
        res.append('</div>')  # api-part
        res.append('')



    if doc.has_key('seealso'):
        p = doc['seealso']
        res.append('<div class="api-part">')
        res.append('<h2 class="api-seealso">See also</h2>')
        res.append('<ul>')
        for i in p:
            res.append('<li><a href="#%s">%s</a></li>' % (htmlEscape(i), htmlEscape(i)))
        res.append('</ul>')
        res.append('</div>')  # api-part
        res.append('')

    if testcase_refs:
        res.append('<div class="api-part">')
        res.append('<h2 class="api-testcases">Related test cases</h2>')
        if testrefs.has_key(doc['name']):
            res.append('<ul>')
            for i in testrefs[doc['name']]:
                res.append('<li>%s</li>' % htmlEscape(i))
            res.append('</ul>')
        else:
            res.append('<p>None.</p>')
        res.append('</div>')  # api-part
        res.append('')

    if not testrefs.has_key(doc['name']):
        res.append('<div class="fixme">This API call has no test cases.</div>')

    if list_tags and len(doc['tags']) > 0:
        # FIXME: placeholder
        res.append('<div class="api-part">')
        res.append('<h2 class="api-tags">Tags</h2>')
        res.append('<p>')
        p = doc['tags']
        for idx, val in enumerate(p):
            if idx > 0:
                res.append(' ')
            res.append(htmlEscape(val))
        res.append('</p>')
        res.append('</div>')  # api-part
        res.append('')

    if doc.has_key('fixme'):
        p = splitNewlineNoLastEmpty(doc['fixme'])
        res.append('<div class="fixme">')
        for i in p:
            res.append(htmlEscape(i))
        res.append('</div>')
        res.append('')

    res.append('</div>')  # api-call div
    return res

def processRawDoc(filename):
    f = open(filename, 'rb')
    res = []
    for line in f.readlines():
        line = stripNewline(line)
        res.append(line)
    f.close()
    res.append('')
    return res

def transformColorizeCode(soup, cssClass, sourceLang):
    for elem in soup.select('pre.' + cssClass):
        input_str = elem.string
        if len(input_str) > 0 and input_str[0] == '\n':
            # hack for leading empty line
            input_str = input_str[1:]

        colorized = sourceHighlight(input_str, sourceLang)

        origTitle = elem.get('title', None)

        # source-highlight generates <pre><tt>...</tt></pre>, get rid of <tt>
        new_elem = BeautifulSoup(colorized, 'lxml').tt    # XXX: parse just a fragment - how?
        new_elem.name = 'pre'
        new_elem['class'] = cssClass

        if origTitle is not None:
            # Preserve title (hover tool tip)
            new_elem['title'] = origTitle

        elem.replace_with(new_elem)

def transformFancyStacks(soup):
    for elem in soup.select('pre.stack'):
        input_str = elem.string
        if len(input_str) > 0 and input_str[0] == '\n':
            # hack for leading empty line
            input_str = input_str[1:]

        new_elem = BeautifulSoup(renderFancyStack(input_str), 'lxml').div  # XXX: fragment?
        elem.replace_with(new_elem)

def transformRemoveClass(soup, cssClass):
    for elem in soup.select('.' + cssClass):
        elem.extract()

def transformReadIncludes(soup, includeDirs):
    for elem in soup.select('*'):
        if not elem.has_key('include'):
            continue
        filename = elem['include']
        del elem['include']
        d = None
        for incdir in includeDirs:
            fn = os.path.join(incdir, filename)
            if os.path.exists(fn):
                f = open(fn, 'rb')
                d = f.read()
                f.close()
                break
        if d is None:
            raise Exception('cannot find include file: ' + repr(filename))

        if filename.endswith('.html'):
            new_elem = BeautifulSoup(d, 'lxml').div
            elem.replace_with(new_elem)
        else:
            elem.string = d

def transformVersionNumber(soup, verstr, tag):
    for elem in soup.select(tag):
        elem.replaceWith(verstr)

def transformCurrentDate(soup):
    curr_date = '%04d-%02d-%02d' % (dt_now.year, dt_now.month, dt_now.day)
    for elem in soup.select('.current-date'):
        elem.replaceWith(curr_date)

def transformAddHrBeforeH1(soup):
    for elem in soup.select('h1'):
        elem.insert_before(soup.new_tag('hr'))

# Add automatic anchors so that a basename from an element with an explicit
# ID is appended with dotted number(s).  Note that headings do not actually
# nest in the document, so this is now based on document order traversal and
# keeping track of counts of headings at different levels, and the active
# explicit IDs at each level.
def transformAddAutoAnchorsNumbered(soup):
    level_counts = [ 0, 0, 0, 0, 0, 0 ]                    # h1, h2, h3, h4, h5, h6
    level_ids = [ None, None, None, None, None, None ]     # explicit IDs
    hdr_tags = { 'h1': 0, 'h2': 1, 'h3': 2, 'h4': 3, 'h5': 4, 'h6': 5 }

    changes = []

    def _proc(root, state):
        idx = hdr_tags.get(root.name, None)
        if idx is None:
            return

        # bump count at matching level and zero lower levels
        level_counts[idx] += 1
        for i in xrange(idx + 1, 6):
            level_counts[i] = 0

        # set explicit ID for current level
        if root.has_key('id'):
            level_ids[idx] = root['id']
            return

        # no explicit ID at current level, clear it
        level_ids[idx] = None

        # figure out an automatic ID: closest explicit ID + dotted
        # numbers to current level

        parts = []
        for i in xrange(idx, -1, -1):  # idx, idx-1, ..., 0
            if level_ids[i] is not None:
                parts.append(level_ids[i])
                break
            parts.append(str(level_counts[i]))
            if i == 0:
                parts.append('doc')  # if no ID in path, use e.g. 'doc.1.2'
        parts.reverse()
        auto_id = '.'.join(parts)

        # avoid mutation: record changes to be made first
        # (adding 'id' would be OK, but this is more flexible
        # if explicit anchors are added instead / in addition
        # to 'id' attributes)
        changes.append((root, auto_id))

    def _rec(root, state):
        if not isinstance(root, Tag):
            return
        _proc(root, state)
        for elem in root.children:
            _rec(elem, state)

    _rec(soup.select('body')[0], {})

    for elem, auto_id in changes:
        elem['id'] = auto_id

# Add automatic anchors where section headings are used to autogenerate
# suitable names.  This does not work very well: there are many subsections
# with the name "Example" or "Limitations", for instance.  Prepending the
# parent name (or rather names of all the parents) would create very long
# names.
def transformAddAutoAnchorsNamed(soup):
    hdr_tags = [ 'h1', 'h2', 'h3', 'h4', 'h5', 'h6' ]

    ids = {}

    def findAutoName(txt):
        # simple name sanitation, not very well thought out; goal is to get
        # nice web-like anchor names from whatever titles are present
        txt = txt.strip().lower()
        if len(txt) > 1 and txt[0] == '.':
            txt = txt[1:]  # leading dot convention for API section names
        txt = txt.replace('c++', 'cpp')
        txt = txt.replace('. ', ' ')  # e.g. 'vs.' -> 'vs'
        txt = txt.replace(', ', ' ')  # e.g. 'foo, bar' -> 'foo bar'
        txt = txt.replace(' ', '_')
        res = ''
        for i,c in enumerate(txt):
            if (ord(c) >= ord('a') and ord(c) <= ord('z')) or \
               (ord(c) >= ord('A') and ord(c) <= ord('Z')) or \
               (ord(c) >= ord('0') and ord(c) <= ord('9') and i > 0) or \
               c in '_':
                res += c
            elif c in '()[]{}?\'"':
                pass  # eat
            else:
                res += '_'
        return res

    for elem in soup.select('*'):
        if not elem.has_key('id'):
            continue
        e_id = elem['id']
        if ids.has_key(e_id):
            print('WARNING: duplicate id %s' % e_id)
        ids[e_id] = True

    # add automatic anchors for every other heading, with priority in
    # naming for higher level sections (e.g. h2 over h3)
    for hdr in hdr_tags:
        for elem in soup.select(hdr):
            if elem.has_key('id'):
                continue  # already has an id anchor
            e_name = elem.text
            a_name = findAutoName(e_name)
            if ids.has_key(a_name):
                print('WARNING: cannot generate automatic anchor name for %s (already exists)' % e_name)
                continue
            ids[a_name] = True
            elem['id'] = a_name

def transformAddHeadingLinks(soup):
    hdr_tags = [ 'h1', 'h2', 'h3', 'h4', 'h5', 'h6' ]
    changes = []

    for elem in soup.select('*'):
        if elem.name not in hdr_tags or not elem.has_key('id'):
            continue

        new_elem = soup.new_tag('a')
        new_elem['href'] = '#' + elem['id']
        new_elem['class'] = 'sectionlink'
        new_elem.string = u'\u00a7'  # section sign

        # avoid mutation while iterating
        changes.append((elem, new_elem))

    for elem, new_elem in changes:
        if elem.has_key('class'):
            elem['class'].append('sectiontitle')
        else:
            elem['class'] = 'sectiontitle'
        elem.append(' ')
        elem.append(new_elem)

def setNavSelected(soup, pagename):
    # pagename must match <li><a> content
    for elem in soup.select('#site-top-nav li'):
        if elem.text == pagename:
            elem['class'] = 'selected'

# FIXME: refactor shared parts

def scanApiCalls(apitestdir):
    re_api_call = re.compile(r'noomiojs_[0-9a-zA-Z_]+')

    res = {}  # api call -> [ test1, ..., testN ]

    tmpfiles = os.listdir(apitestdir)
    for filename in tmpfiles:
        if os.path.splitext(filename)[1] != '.c':
            continue

        f = open(os.path.join(apitestdir, filename))
        data = f.read()
        f.close()

        apicalls = re_api_call.findall(data)
        for i in apicalls:
            if not res.has_key(i):
                res[i] = []
            if filename not in res[i]:
                res[i].append(filename)

    for k in res.keys():
        res[k].sort()

    return res

def createTagIndex(api_docs, used_tags):
    res = []
    res.append('<h1 id="bytag">API calls by tag</h1>')

    for tag in used_tags:
        res.append('<h2 id="taglist-' + htmlEscape(tag) + '">' + htmlEscape(tag) + '</h2>')
        res.append('<ul class="taglist">')
        for doc in api_docs:
            for i in doc['tags']:
                if i != tag:
                    continue
                res.append('<li><a href="#%s">%s</a></li>' % (htmlEscape(doc['name']), htmlEscape(doc['name'])))
        res.append('</ul>')

    return res

def generateApiDoc(apidocdir, apitestdir):
    templ_soup = validateAndParseHtml(readFile('template.html'))
    setNavSelected(templ_soup, 'API')

    # scan api files

    tmpfiles = os.listdir(apidocdir)
    apifiles = []
    for filename in tmpfiles:
        if os.path.splitext(filename)[1] == '.yaml':
            apifiles.append(filename)
    apifiles.sort()
    #print(apifiles)
    print '%d api files' % len(apifiles)

    # scan api testcases for references to API calls

    if apitestdir:
        testrefs = scanApiCalls(apitestdir)
    else:
        testrefs = None
    #print(repr(testrefs))

    # title

    title_elem = templ_soup.select('#template-title')[0]
    del title_elem['id']
    title_elem.string = 'SingleFin API'

    # scan api doc files

    used_tags = []
    api_docs = []   # structure from YAML file directly

    for filename in apifiles:
        apidoc = None
        try:
            with open(os.path.join(apidocdir, filename), 'rb') as f:
                apidoc = yaml.safe_load(f)
            if isinstance(apidoc, (str, unicode)):
                apidoc = None
                raise Exception('parsed as string')
        except:
            print 'WARNING: FAILED TO PARSE API DOC: ' + str(filename)
            print traceback.format_exc()
            pass

        funcname = os.path.splitext(os.path.basename(filename))[0]

        #print(json.dumps(apidoc, indent=4))

        if apidoc is not None:
            if not apidoc.has_key('tags'):
                apidoc['tags'] = []  # ensures tags is present
            apidoc['name'] = funcname  # add funcname automatically

            # add 'introduced' version to tag list automatically
            if apidoc.has_key('introduced'):
                apidoc['tags'].insert(0, apidoc['introduced'])

            if 'omit' in apidoc['tags']:
                print 'Omit API doc: ' + str(funcname)
                continue

            for i in apidoc['tags']:
                assert(i is not None)
                if i not in used_tags:
                    used_tags.append(i)

            api_docs.append(apidoc)

    used_tags.sort()

    # nav

    res = []
    navlinks = []
    navlinks.append(['#build', 'Build'])
    navlinks.append(['#load', 'Load'])
    navlinks.append(['#commandline', 'Command Line'])
    #navlinks.append(['#defines', 'Header definitions'])
    navlinks.append(['#bytag', 'API calls by tag'])
    navlinks.append(['', u'\u00a0'])  # XXX: force vertical space
    for doc in api_docs:
        funcname = doc['name']
        navlinks.append(['#' + funcname, funcname])
    res.append('<ul>')
    for nav in navlinks:
        res.append('<li><a href="' + htmlEscape(nav[0]) + '">' + htmlEscape(nav[1]) + '</a></li>')
    res.append('</ul>')

    nav_soup = validateAndParseHtml('\n'.join(res))
    tmp_soup = templ_soup.select('#site-middle-nav')[0]
    tmp_soup.clear()
    for i in nav_soup.select('body')[0]:
        tmp_soup.append(i)

    # content

    res = []
    res += [ '<h1>API</h1>' ]

    # FIXME: generate from the same list as nav links for these
    res += processRawDoc('../../apps/common/api/intro.html')
    res += processRawDoc('../../apps/common/api/build.html')
    res += processRawDoc('../../apps/common/api/load.html')
    res += processRawDoc('../../apps/common/api/commandline.html')
    #res += processRawDoc('api/concepts.html')
    #res += processRawDoc('api/defines.html')

    # tag index
    res += createTagIndex(api_docs, used_tags)

    # api docs
    for doc in api_docs:
        # FIXME: Here we'd like to validate individual processApiDoc() results so
        # that they don't e.g. have unbalanced tags.  Or at least normalize them so
        # that they don't break the entire page.

        data = None
        try:
            data = processApiDoc(doc, testrefs, used_tags)
            res += data
        except:
            print repr(data)
            print 'FAIL: ' + repr(doc['name'])
            raise

    print('used tags: ' + repr(used_tags))

    content_soup = validateAndParseHtml('\n'.join(res))
    tmp_soup = templ_soup.select('#site-middle-content')[0]
    tmp_soup.clear()
    for i in content_soup.select('body')[0]:
        tmp_soup.append(i)
    tmp_soup['class'] = 'container content'

    return templ_soup

def generateIndexPage():
    templ_soup = validateAndParseHtml(readFile('template.html'))
    index_soup = validateAndParseHtml(readFile('index/index.html'))
    setNavSelected(templ_soup, 'Home')

    title_elem = templ_soup.select('#template-title')[0]
    del title_elem['id']
    title_elem.string = 'NoomioJS'

    tmp_soup = templ_soup.select('#site-middle-content')[0]
    tmp_soup.clear()
    for i in index_soup.select('body')[0]:
        tmp_soup.append(i)
    tmp_soup['class'] = 'container content'

    return templ_soup

def generateDownloadPage(releases_filename):
    templ_soup = validateAndParseHtml(readFile('template.html'))
    down_soup = validateAndParseHtml(readFile('download/download.html'))
    setNavSelected(templ_soup, 'Download')

    # Flip download list, preferred by most users
    tbody = down_soup.select('#releases tbody')[0]
    assert(tbody is not None)
    dl_list = tbody.select('tr')
    for dl in dl_list:
        dl.extract()
    dl_list.reverse()
    for dl in dl_list:
        tbody.append(dl)

    title_elem = templ_soup.select('#template-title')[0]
    del title_elem['id']
    title_elem.string = 'Downloads'

    if fancy_releaselog:
        # fancy releaselog
        rel_data = rst2Html(os.path.abspath(os.path.join('..', 'RELEASES.rst')))
        rel_soup = BeautifulSoup(rel_data, 'lxml')
  
        planned = rel_soup.select('#planned')[0]
        for elem in planned.select('h1'):
            elem.extract()
        for elem in planned.select('h2'):
            new_tag = rel_soup.new_tag("span")
            elem.append(new_tag)
            elem['class'] = 'badge badge-warning'

        plannedlog_elem = down_soup.select('#plannedlog')[0]
        plannedlog_elem.insert_after(planned)

        released = rel_soup.select('#released')[0]
        # massage the rst2html generated HTML to be more suitable
        for elem in released.select('h1'):
            elem.extract()
        for elem in released.select('h2'):
            new_tag = rel_soup.new_tag("span")
            elem.append(new_tag)
            elem['class'] = 'badge badge-primary'
        # Extract and reinsert release versions to reverse their order
        # (newest release first).
        rel_list = released.select('.section')
        for rel in rel_list:
            rel.extract()
        rel_list.reverse()
        for rel in rel_list:
            released.append(rel)

        releaselog_elem = down_soup.select('#releaselog')[0]
        releaselog_elem.insert_after(released)
    else:
        # plaintext releaselog
        releaselog_elem = down_soup.select('#releaselog')[0]
        pre_elem = down_soup.new_tag('pre')
        releaselog_elem.append(pre_elem)
        f = open(releases_filename, 'rb')
        pre_elem.string = f.read().decode('utf-8')
        f.close()

    # automatic md5sums for downloadable files
    # <tr><td class="reldate">2013-09-21</td>
    #     <td class="filename"><a href="duktape-0.6.0.tar.xz">duktape-0.6.0.tar.xz</a></td>
    #     <td class="description">alpha, first round of work on public API</td>
    #     <td class="hash">fa384a42a27d996313e0192c51c50b4a</td></tr>

    for tr in down_soup.select('tr'):
        tmp = tr.select('.filename')
        if len(tmp) != 1:
            continue
        href = tmp[0].select('a')[0]['href']
        hash_elem = tr.select('.hash')[0]
        hash_elem.string = getFileMd5(os.path.abspath(os.path.join('..', 'noomiojs-releases', href))) or '???'

    tmp_soup = templ_soup.select('#site-middle-content')[0]
    tmp_soup.clear()
    for i in down_soup.select('body')[0]:
        tmp_soup.append(i)
    tmp_soup['class'] = 'container content'

    return templ_soup

def generateGuide():
    templ_soup = validateAndParseHtml(readFile('template.html'))
    setNavSelected(templ_soup, 'Guide')

    title_elem = templ_soup.select('#template-title')[0]
    del title_elem['id']
    title_elem.string = 'NoomioJS Programmer\'s Guide'

    # nav

    # res = []
    # navlinks = []
    # navlinks.append(['#examples', 'Examples'])

    # res.append('<ul>')
    # for nav in navlinks:
    #     res.append('<li><a href="' + htmlEscape(nav[0]) + '">' + htmlEscape(nav[1]) + '</a></li>')
    # res.append('</ul>')

    # nav_soup = validateAndParseHtml('\n'.join(res))
    # tmp_soup = templ_soup.select('#site-middle-nav')[0]
    # tmp_soup.clear()
    # for i in nav_soup.select('body')[0]:
    #     tmp_soup.append(i)

    # content

    res = []
    res += [ '<h1>Programmer\'s Guide</h1>' ]

    res += processRawDoc('guide/intro.html')
    res += processRawDoc('guide/examples.html')



    content_soup = validateAndParseHtml('\n'.join(res))
    tmp_soup = templ_soup.select('#site-middle-content')[0]
    tmp_soup.clear()
    for i in content_soup.select('body')[0]:
        tmp_soup.append(i)
    tmp_soup['class'] = 'container content'

    return templ_soup

def generateExamples():
    templ_soup = validateAndParseHtml(readFile('template.html'))
    setNavSelected(templ_soup, 'Examples')

    title_elem = templ_soup.select('#template-title')[0]
    del title_elem['id']
    title_elem.string = 'Javascript Examples'

    # nav

    res = []
    navlinks = []
    navlinks.append(['#hello-world', 'Hello World'])
    navlinks.append(['#timer', 'Timer'])
    navlinks.append(['#i2c', 'I2C'])

    res.append('<ul>')
    for nav in navlinks:
        res.append('<li><a href="' + htmlEscape(nav[0]) + '">' + htmlEscape(nav[1]) + '</a></li>')
    res.append('</ul>')

    nav_soup = validateAndParseHtml('\n'.join(res))
    tmp_soup = templ_soup.select('#site-middle-nav')[0]
    tmp_soup.clear()
    for i in nav_soup.select('body')[0]:
        tmp_soup.append(i)

    # content

    res = []
    res += [ '<h1>Javascript Examples</h1>' ]

   
    res += processRawDoc('examples/intro.html')
    res += processRawDoc('examples/hello-world.html')
    res += processRawDoc('examples/timer.html')
    res += processRawDoc('examples/i2c.html')

    content_soup = validateAndParseHtml('\n'.join(res))
    tmp_soup = templ_soup.select('#site-middle-content')[0]
    tmp_soup.clear()
    for i in content_soup.select('body')[0]:
        tmp_soup.append(i)
    tmp_soup['class'] = 'container content'

    return templ_soup

def generateDfotaPage():
    templ_soup = validateAndParseHtml(readFile('template.html'))
    setNavSelected(templ_soup, 'Dfota')
    index_soup = validateAndParseHtml(readFile('dfota/index.html'))

    title_elem = templ_soup.select('#template-title')[0]
    del title_elem['id']
    title_elem.string = 'Dfota'

    tmp_soup = templ_soup.select('#site-middle-content')[0]
    tmp_soup.clear()
    for i in index_soup.select('body')[0]:
        tmp_soup.append(i)
    tmp_soup['class'] = 'container content'

    return templ_soup

def generateStyleCss():
    styles = [
        'reset.css',
        'style-content.css',
        'style-top.css',
        'style-middle.css',
        'style-index.css',
        'style-api.css',
        'highlight.css',
        'bootstrap.min.css',
    ]

    style = ''
    for i in styles:
        style += '/* === %s === */\n' % i
        style += readFile(i)

    return style

def postProcess(soup, includeDirs, autoAnchors=False, headingLinks=False, duktapeVersion=None, noomiojsVersion=None,
    baselineVersion='3.0', colorize =True):
    # read in source snippets from include files
    if True:
        transformReadIncludes(soup, includeDirs)

    # version number
    if True:
        transformVersionNumber(soup, duktapeVersion, '.duktape-version')
        transformVersionNumber(soup, noomiojsVersion, '.noomiojs-version')
        transformVersionNumber(soup, baselineVersion, '.baseline-version')

    # current date
    if True:
        transformCurrentDate(soup)

    # add <hr> elements before all <h1> elements to improve readability
    # in text browsers
   # if True:
   #     transformAddHrBeforeH1(soup)

    # add automatic anchors to all headings (as long as they don't conflict
    # with any manually assigned "long term" ids)
    if autoAnchors:
        transformAddAutoAnchorsNumbered(soup)

    if headingLinks:
        transformAddHeadingLinks(soup)

    if colorize:
        transformColorizeCode(soup, 'c-code', 'c')
        transformColorizeCode(soup, 'ecmascript-code', 'javascript')

    if fancy_stack:
        transformFancyStacks(soup)

    if remove_fixme:
        transformRemoveClass(soup, 'fixme')

    return soup

def writeFile(name, data):
    f = open(name, 'wb')
    f.write(data)
    f.close()

def scrapeDuktapeVersion():
    f = open(os.path.join('../../../', 'src-input', 'duktape.h.in'))
    re_ver = re.compile(r'^#define DUK_VERSION\s+(\d+)L?\s*$')
    for line in f:
        line = line.strip()
        m = re_ver.match(line)
        if m is None:
            continue
        raw_ver = int(m.group(1))
        str_ver = '%d.%d.%d' % ( raw_ver / 10000, raw_ver / 100 % 100, raw_ver % 100)
    f.close()
    if raw_ver is None:
        raise Exception('cannot scrape Duktape version')
    return str_ver, raw_ver

def scrapeNoomioJSVersion():
    f = open(os.path.join('..', 'src', 'noomiojs.h.in'))

    re_ver = re.compile(r'^#define NOOMIOJS_VERSION\s+(\d+)[.](\d+)[.](\d+)-?\s*$')
    re_ver_dirty = re.compile(r'^#define NOOMIOJS_VERSION\s+(\d+)[.](\d+)[.](\d+)-dirty?\s*$')

    for line in f:
        line = line.strip()
        m = re_ver.match(line)
        if m is None:
            m = re_ver_dirty.match(line)
            if m is None:
                continue
            else:
              raise Exception('Dirty version!\r\nCommit all your changes and try again.')   
        raw_ver = m.group(1) + "." + m.group(2) + "." + m.group(3)
        str_ver = raw_ver
    f.close()
    if raw_ver is None:
        raise Exception('cannot scrape noomio version')
    return str_ver, raw_ver


def main():
    outdir = sys.argv[1]; assert(outdir)
    apidocdir = '../../apps/common/api'
    apitestdir = '../../apps/tests'
    #guideincdirs = [ './guide', '../examples/guide' ]
    #exampleincdirs = [ './examples' ]
    apiincdirs = [ '../../apps/common/api' ]
    out_charset = 'utf-8'
    releases_filename = None

   # duk_verstr, duk_verint = scrapeDuktapeVersion()
   # noomiojs_verstr, noomio_verint = scrapeNoomioJSVersion()
   # print 'Duk Scraped version number: ' + duk_verstr
   # print 'Noomiojs Scraped version number: ' + noomiojs_verstr

    print 'Generating style.css'
    data = generateStyleCss()
    writeFile(os.path.join(outdir, 'style.css'), data)
    #writeFile(os.path.join(outdir, 'reset.css'), readFile('reset.css'))
    #writeFile(os.path.join(outdir, 'highlight.css'), readFile('highlight.css'))

    print 'Generating api.html'
    soup = generateApiDoc(apidocdir, apitestdir)
    soup = postProcess(soup, apiincdirs, autoAnchors=True, headingLinks=True )
    writeFile(os.path.join(outdir, 'api.html'), soup.encode(out_charset))

    #print 'Generating guide.html'
    #soup = generateGuide()
    #soup = postProcess(soup, guideincdirs, autoAnchors=True, headingLinks=True, duktapeVersion=duk_verstr, noomiojsVersion=noomiojs_verstr)
    #writeFile(os.path.join(outdir, 'guide.html'), soup.encode(out_charset))

    #print 'Generating index.html'
    #soup = generateIndexPage()
    #soup = postProcess(soup, None, duktapeVersion=duk_verstr, noomiojsVersion=noomiojs_verstr)
    #writeFile(os.path.join(outdir, 'index.html'), soup.encode(out_charset))

    
    #print 'Generating examples.html'
    #soup = generateExamples()
    #soup = postProcess(soup, exampleincdirs, autoAnchors=True, headingLinks=True, duktapeVersion=duk_verstr, noomiojsVersion=noomiojs_verstr)
    #writeFile(os.path.join(outdir, 'examples.html'), soup.encode(out_charset))

    #print 'Generating download.html'
    #soup = generateDownloadPage(releases_filename)
    #soup = postProcess(soup, None, duktapeVersion=duk_verstr, noomiojsVersion=noomiojs_verstr)
    #writeFile(os.path.join(outdir, 'download.html'), soup.encode(out_charset))

    # benchmarks.html is generated dynamically

    print 'Copying misc files'
    for i in [ 'robots.txt',
               'LICENSE.txt'
                ]:
        shutil.copyfile(os.path.join('./', i), os.path.join(outdir, os.path.basename(i)))

    print 'Copying assets'
    outdir_assets = os.path.join(outdir, 'assets')
    if os.path.isdir(outdir_assets) is False:  
        os.mkdir(outdir_assets) 
    for i in os.listdir(os.path.join('./', 'assets/icons')):
        shutil.copyfile(os.path.join('./', 'assets/icons', i), os.path.join(outdir_assets, i))

    #print 'Copying release binaries'
    #for i in os.listdir(os.path.join('..', 'noomiojs-releases')):
    #    if re.match(r'^noomiojs-.*?.tar$', i) is None:
    #        continue
    #    shutil.copyfile(os.path.join('..', 'noomiojs-releases', i), os.path.join(outdir, i))

    #print 'Copying dukweb.js files'
    #for i in [ '../dukweb.js',
    #           '../jquery-1.11.2.js',
    #           '../dukweb/dukweb.css',
    #           '../dukweb/dukweb.html' ]:
    #    shutil.copyfile(os.path.join('./', i), os.path.join(outdir, os.path.basename(i)))

    #print 'Copying benchmarks.html dependencies'
    #shutil.copyfile(os.path.join('../lz-string/libs/lz-string.js'), os.path.join(outdir, 'lz-string.js'))

if __name__ == '__main__':
    main()

