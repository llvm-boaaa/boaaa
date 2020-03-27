#!bin/bash
echo ""
echo "Info: use bash-4 or newer, otherwise the script will fail."
echo ""

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd)"

true=true
false=false

#read config
typeset -A config #init arrays
#typeset -A versions
config=( #init default values
    [bc_foulder]=../bc_sources
    [wllvm_path]=/usr/local/bin
)

while read line
do
    if echo $line | grep -F = &>/dev/null
    then
        if [[ "$line" == versions=* ]]
        then
            read -r -a versions <<< $( echo $line | cut -d "(" -f 2 | cut -d ")" -f 1)
        else
            echo $line
            
            varname=$(echo $line | cut -d '=' -f 1)
            config[$varname]=$(echo $line | cut -d '=' -f 2-)
        fi
    fi
done < ${SCRIPT_DIR}/config.conf

echo "========================================"
echo "config:"
echo "bc_foulder: ${config[bc_foulder]}"
echo "wllvm_path: ${config[wllvm_path]}"

echo ""
echo "versions:   (${versions[@]})"
for ver in ${versions[@]}
do
varname=clang_${ver}_path
build_path=build_${ver}
echo "version $ver:"
echo "clang path: ${config[${varname}]}"
echo "build path: ${config[${build_path}]}"
echo ""
done

foulder_set=$false

#commandline
if [ "$1" == "-r" ] 
then
    if [ -z $2 ]
    then
    echo ""
    else
        foulder_set=$true 
        tmp=$PWD        
        cd $2           
        FOULDER=$PWD    
        cd $tmp
    fi         
else
    if [ -z $1 ]
    then 
        echo ""
    else
        foulder_set=$true
        FOULDER=$1
    fi
fi

echo "foulder: $FOULDER"
basename=$(basename $FOULDER )
echo "basename: ${basename}"

if [ $foulder_set = $true ]
then

#build -- use CMake, Make, configure(with make), automake

for ver in ${versions[@]}
do 
     #############################
 ####                             ####
#           version - loop            #

echo "================================ - v $ver"

CMAKE=$false
MAKE=$false
AUTOCONF=$false
CONFIGURE=$false
SUP_BS=$false

for entry in `ls $FOULDER` 
do 
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

build_foulder=${config[build_${ver}]}
clang_foulder=${config[clang_${ver}_path]}

export LLVM_COMPILER_PATH=$clang_foulder
export CC=${config[wllvm_path]}/wllvm
export CXX=${config[wllvm_path]}/wllvm++
export LLVM_COMPILER=clang

if [[ -z $build_foulder ]] || [[ -z $clang_foulder ]]
then
    continue
fi

if [ ! -d $build_foulder ]
then 
    mkdir -p $build_foulder
fi

echo "clean..."
rm -rf $build_foulder
echo "done."

if [ ! -d $build_foulder ]
then 
    mkdir -p $build_foulder
fi

#copy build files for each version to its build destination
if [[ $AUTOCONF = $true ]] || [[ $CONFIGURE = $true ]] || [[ $MAKE = $true ]]
then
echo "copy files"
    cp -r $FOULDER $build_foulder
fi

if [[ $CMAKE = $true ]] && [[ $MAKE = $false ]]
then
    echo "cmake configure..."
    cd $build_foulder
    wllvm_path=${config[wllvm_path]}/wllvm
    wllvmpp_path=${config[wllvm_path]}/wllvm++
    echo "wllvm path:   " $wllvm_path
    echo "wllvm++ path: " $wllvmpp_path
    echo "clang path:   " $LLVM_COMPILER_PATH
    cmake $FOULDER -G"Unix Makefiles" -DCMAKE_C_COMPILER=$wllvm_path -DCMAKE_CXX_COMPILER=$wllvmpp_path
    MAKE=$true
fi

if [[ $AUTOCONF = $true ]] && [[ $MAKE = $false ]]
then
    echo "autoconf..."
    cd $build_foulder
    autoreconf -i -f
fi

if [[ $CONFIGURE = $true ]] && [[ $MAKE = $false ]]
then
    echo "configure..."
    cd $build_foulder
    ./configure
    MAKE=$true
fi

#build bc-file
if [[ $MAKE = $true ]]
then
    echo "make build..."
    cd $build_foulder
     make
    echo "done."
fi

#generate bc from builded files

#write all libs in array
mapfile -d $'\0' liblist < <(find -E $build_foulder -regex '.*/.*[.](a|dylib)' -print | tr '\n' ' ' )

filebase=${config[bc_foulder]}/$basename/$ver
if [ ! -d $filebase ]
then 
    mkdir -p $filebase
fi

for lib in ${liblist[@]}
do
    ${config[wllvm_path]}/extract-bc -b $lib
    filename=$(basename $lib)

    cp "${lib}.bc" "${filebase}/${filename}.bc"
    echo "copied ${filename}"
done

done
#           version - loop            #
 ####                             ####
     #############################
else
    echo "skip generation, because foulder wasn't defined"
fi