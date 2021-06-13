git clone https://bitbucket.org/chengyinwu/v3.git
rm -rf v3/engine
cp -rf SoCV_v3_engine v3/engine
cp Makefile.in.diff v3/src
cd v3/engine/; ./boolector.script; ./minisat.script; ./quteRTL.script; cd ../
cd src; patch Makefile.in < Makefile.in.diff; cd ..; make;
