# coding=utf-8

'''
Created on 2011-11-19

@author: weizili

@purpose: Crawling English words from dict.youdao.com
'''



import urllib
import string
from sgmllib import SGMLParser
import HTMLParser


from ut import *
from word import *
        
class DictParser(SGMLParser):
    
    def __init__(self, wordname):
        self._word = Word()
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
            if len(self._tmp_sentence) == 0:
                self._tmp_sentence = string.strip(text)
            else:
                self._tmp_sentence += ' ' + string.strip(text)
        
        
    def output(self):
        s = self._word.to_string()
        print s
        return s 


if __name__ == '__main__':
    #url = 'http://www.iciba.com/abdicate/'
    #url = 'http://dict.cn/abdicate/'
    
    #test_list()
    
    wordnames = ['success', 'abdicate', 'perfect', 'frequently', 'abundant']
    for wordname in wordnames:
        url = 'http://dict.youdao.com/search?q=%s&ue=utf8' % wordname 
        sock = urllib.urlopen(url)
        htmlSource = sock.read()
        sock.close()
        #print htmlSource
        
        parser = DictParser(wordname)
        parser.feed(htmlSource)
        parser.output()
        
        f = open('original_word_html/' + wordname + '.html', 'w+')
        f.write(htmlSource)
        f.close()
        
        