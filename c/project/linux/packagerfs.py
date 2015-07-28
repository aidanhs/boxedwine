#!/usr/bin/python
import sys, os, subprocess

def error(s):
    print >> sys.stderr, s
    sys.exit(1)

def getfiletext(fn):
    try:
        f = open(fn, 'r')
        txt = f.read()
    except Exception, e:
        error('Error reading file: %s' % (str(e)))
    f.close
    return txt

try:
    txt = getfiletext('boxedwine.js')
    
    injectCode = """\
             
        function processPackageData(arrayBuffer) {
        Module.finishedDataFileDownloads++;
        assert(arrayBuffer, 'Loading data file failed.');
        
        setGlobalArrayBuffer(arrayBuffer);
        }
        function postProcessPackageData(arrayBuffer){
        """
    txt = txt.replace("function processPackageData(arrayBuffer){Module.finishedDataFileDownloads++;assert(arrayBuffer,\"Loading data file failed.\");", injectCode)
    
    startToken = 'function postProcessPackageData(arrayBuffer){'
    endToken = """Module[\"addRunDependency\"](\"datafile_boxedwine.data\");"""
    startIndex  = txt.index(startToken)
    endIndex = txt.index(endToken)
    beforeTxt = txt[0:startIndex]
    afterTxt = txt[endIndex:]
    
    txt = beforeTxt  + afterTxt
    
    startToken2 = 'function runWithFS(){function assert(check,msg){if(!check)throw msg+(new Error).stack}'
    endToken2 = "function processPackageData(arrayBuffer) {"
    startIndex2  = txt.index(startToken2) + len(startToken2)
    endIndex2 = txt.index(endToken2)
    beforeTxt2 = txt[0:startIndex2]
    afterTxt2 = txt[endIndex2:]
    
    txt = beforeTxt2  + afterTxt2
    
    
    outf = open('package.js', 'w')
    outf.write(txt)
    outf.close



except Exception as e:
    print("error {0}".format(str(e)))
    error('Error ')

