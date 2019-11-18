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
AUTOCONF=$false
CONFIGURE=$false
SUP_BS=$false

for entry in `ls $FOULDER` 
do 
    echo $entry
    if [[ "$entry" == "CMakeLists.txt" ]] 
    then
        CMAKE=$true
        SUP_BS=$true
    elif [[ "$entry" == "MakeFile" ]] || [[ "$entry" == "makefile" ]]
    then
        MAKE=$true
        SUP_BS=$true
    elif [[ "$entry" == "configure" ]]
    then
        CONFIGURE=$true
        SUP_BS=$true
    elif [[ "$entry" == "configure.ac" ]] || [[ "$entry" == "configure.in" ]]
    then
        AUTOCONF=$true
        CONFIGURE=$true
        SUP_BS=$true
    fi
done

if [[ $SUP_BS = $false ]] 
then
    echo "Error: No Cmake or Makefile found"
fi

CC=wllvm CXX=wllvm++
export LLVM_COMPILER=clang

if [[ $CMAKE = $true ]] && [[ $MAKE = $false ]]
then
    cd $FOULDER                                 
    mkdir -p cmake-build 
    cd cmake-build     
    FOULDER=$PWD
    #clean cmake-build
    rm -rf * .*
    ls
    cmake .. -G"Unix Makefiles" -DCMAKE_C_COMPILER=/usr/local/bin/wllvm -DCMAKE_CXX_COMPILER=/usr/local/bin/wllvm++
    MAKE=$true
fi

if [[ $AUTOCONF = $true ]] && [[ $MAKE = $false ]]
then
    cd $FOULDER
    autoreconf -i
fi

if [[ $CONFIGURE = $true ]] && [[ $MAKE = $false ]]
then
    cd $FOULDER
    #for file in `ls $FOULDER` 
    #do 
    #    chmod +x $file
    #done
    CC=wllvm CXX=wllvm++ ./configure
    MAKE=$true
fi

#build bc-file
if [[ $MAKE = $true ]]
then
    cd $FOULDER
    export LLVM_COMPILER=clang
    CC=wllvm CXX=wllvm++ make
fi