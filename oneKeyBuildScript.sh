make
cd ./out/
cp -rf ../script ./
strip ./lib/* ./bin/*
chmod +x -R ./bin ./script
tar zcvf  LiteOs.tar.gz ./bin ./lib ./script 