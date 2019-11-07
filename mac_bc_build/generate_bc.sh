#Variablen
if [ "$1" == "-r" ] 
then
    tmp=$PWD        
    cd $2           
    FOULDER=$PWD    
    cd $tmp         
else
    FOULDER=$1
fi

echo "Foulder: $FOULDER"

true=true
false=false

CMAKE=$false
MAKE=$false

for entry in `ls $FOULDER` 
do 
    echo $entry
    if [[ "$entry" == "CMakeLists.txt" ]] 
    then
        CMAKE=$true
    elif [[ "$entry" == "" ]] 
    then
        MAKE=$true
    fi
done

if [[ $CMAKE = $false ]] && [[ $MAKE = $false ]] 
then
    echo "Error: No Cmake or Makeefile found"
fi

CC=wllvm CXX=wllvm++
export LLVM_COMPILER=clang

if [[ $CMAKE = $true ]] && [[ $MAKE = $false ]]
then
    cd $FOULDER                                 
    mkdir -p cmake-build 
    cd cmake-build      
    #clean cmake-build
    rm -rf * .*
    ls
    cmake .. -G"Unix Makefiles" -DCMAKE_C_COMPILER=/usr/local/bin/wllvm -DCMAKE_CXX_COMPILER=/usr/local/bin/wllvm++
    MAKE=$true
fi


#build bc-file
if [[ $MAKE = $true ]]
then
    export LLVM_COMPILER=clang
    CC=wllvm CXX=wllvm++ make
fi