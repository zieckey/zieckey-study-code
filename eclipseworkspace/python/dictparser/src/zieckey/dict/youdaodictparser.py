# coding=utf-8

'''
Created on 2011-11-19

@author: weizili

@purpose: Crawling English words from dict.youdao.com
'''




from sgmllib import SGMLParser
import HTMLParser
import glob, os, string, sys, random
from xml.dom import minidom
from xml.etree import ElementTree

import word
        
class DictParser(SGMLParser):
    
    def __init__(self, wordname):
        self._word = word.Word()
        self._word.set_name(wordname)
        SGMLParser.__init__(self)
        
        self._phonetic_found = 0

        self._etcTrans_found = 0
        
        self._etcTrans_li_found = 0

        self._p_additional_found = 0
        
        self._synonyms_found = 0
        self._synonyms_a_found = 0
        
        self._tPowerTrans_found = 0
        self._tPowerTrans_ul_sense_ex_found = 0
        self._tPowerTrans_ul_sense_ex_p_found = 0
        
        self._examplesToggle_found = 0
        self._examplesToggle_bilingual_found = 0
        self._examplesToggle_bilingual_p_found = 0
        self._examplesToggle_bilingual_p_noattrs_found = 0 #用来解决“报错”这个<p>
        
        self._tmp_sentence_list = []
        self._tmp_sentence = ''
        
        
    def unknown_starttag(self, tag, attrs):
        
        #phonetic
        if tag == 'span':
            for (k,v) in attrs:
                if k == 'class' and v == 'phonetic':
                    self._phonetic_found = 1
                    break
                    
        #word meaning
        if tag == 'div' and ('id', 'etcTrans') in attrs:
            self._etcTrans_found = 1
            
        elif self._etcTrans_found and tag == 'li':
            self._etcTrans_li_found = 1
            
        elif tag == 'p' and ('id', 'additional') in attrs:
            self._p_additional_found = 1
            
        elif tag == 'div' and ('id', 'synonyms') in attrs:
            self._synonyms_found = 1
            
        elif self._synonyms_found and tag == 'a':
            self._synonyms_a_found = 1
        
        elif tag == 'div' and ('id', 'tPowerTrans') in attrs:
            self._tPowerTrans_found = 1
            
        elif tag == 'ul' and self._tPowerTrans_found and ('class','sense-ex') in attrs:
            self._tPowerTrans_ul_sense_ex_found = 1
            
        elif tag == 'p' and self._tPowerTrans_ul_sense_ex_found:
            self._tPowerTrans_ul_sense_ex_p_found = 1
        
        elif tag == 'div' and ('id', 'examplesToggle') in attrs:
            self._examplesToggle_found = 1
        elif tag == 'div' and ('id', 'bilingual') in attrs and self._examplesToggle_found:
            self._examplesToggle_bilingual_found = 1
        elif tag == 'p' and self._examplesToggle_bilingual_found:
            self._examplesToggle_bilingual_p_found += 1
            if len(attrs) == 0:
                self._examplesToggle_bilingual_p_noattrs_found = 1
            
            
    def unknown_endtag(self, tag):         
        # 对每一个结束标记进行调用，例如，对于 </pre>, 标记将是 "pre"。
        # 重构原始结束标记。
        if tag == 'span' and self._phonetic_found:
            self._phonetic_found = 0
            
        elif self._etcTrans_found and tag == 'div':
            self._etcTrans_found = 0
            
        elif self._etcTrans_found and tag == 'li' and self._etcTrans_li_found:
            self._etcTrans_li_found = 0
            
        elif self._p_additional_found and tag== 'p':
            self._p_additional_found = 0
            
        elif self._synonyms_found and tag == 'div':
            self._synonyms_found = 0
            
        elif self._synonyms_found and tag == 'a' and self._synonyms_a_found:
            self._synonyms_a_found = 0
            
        elif tag == 'div' and self._tPowerTrans_found:
            self._tPowerTrans_found = 0
            
        elif tag == 'ul' and self._tPowerTrans_found and self._tPowerTrans_ul_sense_ex_found:
            self._tPowerTrans_ul_sense_ex_found = 0
            
        elif tag == 'p' and self._tPowerTrans_ul_sense_ex_found and self._tPowerTrans_ul_sense_ex_p_found:
            self._tPowerTrans_ul_sense_ex_p_found = 0
                
        elif tag == 'p' and self._examplesToggle_bilingual_found and self._examplesToggle_bilingual_p_found > 0:
            self._examplesToggle_bilingual_p_found -= 1
            if self._examplesToggle_bilingual_p_noattrs_found:
                self._examplesToggle_bilingual_p_noattrs_found = 0
                self._tmp_sentence_list.append(string.strip(self._tmp_sentence))
                self._tmp_sentence = ''
                if len(self._tmp_sentence_list) == 2:
                    self._word.add_sentence(self._tmp_sentence_list[0], self._tmp_sentence_list[1])
                    self._tmp_sentence_list.pop()
                    self._tmp_sentence_list.pop()
            
        elif tag == 'div' and self._examplesToggle_found:
            self._examplesToggle_bilingual_found = 0
            
        elif tag == 'div' and self._examplesToggle_found:
            self._examplesToggle_found = 0
            
        
                
        
    def handle_data(self, text):                                        
        # 覆盖
        # 对HTML源代码中的每个文本块进行调用
        if self._phonetic_found:
            self._word.set_british_phonetic(text)
            
        elif self._etcTrans_found and self._etcTrans_li_found:
            self._word.add_meaning(text)
        
        elif self._p_additional_found:
            self._word.add_additional(HTMLParser.HTMLParser().unescape(text))
            
        elif self._synonyms_a_found:
            self._word.add_synonym(text)
        
        elif self._tPowerTrans_ul_sense_ex_p_found:
            self._tmp_sentence_list.append(text)
            if len(self._tmp_sentence_list) == 2:
                self._word.add_sentence(self._tmp_sentence_list[0], self._tmp_sentence_list[1])
                self._tmp_sentence_list.pop()
                self._tmp_sentence_list.pop()
                
        elif self._examplesToggle_bilingual_p_noattrs_found:
            if len(self._tmp_sentence_list) == 0:
                if len(self._tmp_sentence) == 0:
                    self._tmp_sentence = string.strip(text)
                else:
                    self._tmp_sentence += ' ' + string.strip(text)#The English sentence needs space between words
            else:
                self._tmp_sentence += string.strip(text)#The Chinese sentence does not need space between words
        
        
    def output(self):
        s = self._word.to_string()
        print s
        return s 
    
    def get_word(self):
        return self._word

def test_special_word():
    special_words = ['vertex', 'verify']
    
    word_files = []
    for word in special_words:
        word_files.append('./gre/%s.html' % word)
    
    for word_file in word_files:
        filebasename = os.path.basename(word_file)
        (wordname, ext) = os.path.splitext(filebasename)
        
        fp = open(word_file)
        htmlSource = fp.read()
        fp.close()
        
        parser = DictParser(wordname)
        parser.feed(htmlSource)
        parser.output()

def listAllFiles( dirpath ):
    """
        List all the files in this directory recursively
        and return a the file list
    """
    if not os.path.isdir(dirpath):
        return []

    allfiles = []
    for root, dirs, files in os.walk(dirpath):
        for f in files:
            ff = os.path.join(root, f)
            allfiles.append(ff)

    return allfiles

def parse_one_tag(tagname, basedir):
    all_words = [] # word.Word
    
    bad_words = ''
        
    youdao_wordbook_xml = ElementTree.Element('wordbook')
    word_files = glob.glob('./%s/*.html' % basedir)
    for word_file in word_files:
        filebasename = os.path.basename(word_file)
        (wordname, ext) = os.path.splitext(filebasename)
                
        fp = open(word_file)
        htmlSource = fp.read()
        fp.close()
        
        parser = DictParser(wordname)
        parser.feed(htmlSource)
        parser.output()
        
        if parser.get_word().is_ok():
            youdao_wordbook_xml.append(parser.get_word().to_xml_element(tagname))
            all_words.append(parser.get_word());
        else:
            bad_words += parser.get_word().get_name() + '\n'
    
    #the whole words
    f = open(tagname + '_youdao.xml', 'w+')
    xmlstr= ElementTree.tostring(youdao_wordbook_xml, 'utf-8')
    reparsed_xmldoc = minidom.parseString(xmlstr)
    f.write(reparsed_xmldoc.toprettyxml(indent='\t', encoding="utf-8"))
    f.close()

    #shuffle
    random.shuffle(all_words)

    #split the whole words
    total_words_count = len(all_words)
    UNIT_COUNT = 600
    count = total_words_count / UNIT_COUNT 
    if total_words_count % UNIT_COUNT != 0:
        count += 1
        
    orig_tagname = tagname
    for i in range(0, count):
        tagname = "%s%d" % (orig_tagname, i)
        youdao_wordbook_xml = ElementTree.Element('wordbook')
        f = open(tagname + '_youdao.xml', 'w+')
        for j in range(0, UNIT_COUNT):
            index = i * UNIT_COUNT + j
            if index < total_words_count:
                youdao_wordbook_xml.append(all_words[index].to_xml_element(tagname))
            else:
                break;
        xmlstr= ElementTree.tostring(youdao_wordbook_xml, 'utf-8')
        reparsed_xmldoc = minidom.parseString(xmlstr)
        f.write(reparsed_xmldoc.toprettyxml(indent='\t', encoding="utf-8"))
        f.close()

    
    f = open(tagname + '_bad_word.txt', 'w+')
    f.write(bad_words)
    f.close()    

if __name__ == '__main__':
    #url = 'http://www.iciba.com/abdicate/'
    #url = 'http://dict.cn/abdicate/'
#    if len(sys.argv) != 2:
#        print 'Usage : %s tag_name' % sys.argv[0]
#        print 'For example : %s gre' % sys.argv[0]
#        print 'Note: we you the tag_name as the default directory to find the orignal html files'
#        sys.exit(-1)

    
    tagnames = []
    for root, dirs, files in os.walk('./original_youdao_html'):
        if os.path.basename(root) == 'original_youdao_html':
            tagnames = dirs
            break
            
    for tagname in tagnames:
        parse_one_tag(tagname, './original_youdao_html/' + tagname )
    
    
