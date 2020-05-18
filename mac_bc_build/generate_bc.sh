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

#read config
while read line
do
    if echo $line | grep -F = &>/dev/null
    then
        if [[ "$line" == versions=* ]]
        then
            read -r -a versions <<< $( echo $line | cut -d "(" -f 2 | cut -d ")" -f 1)
        else
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
export LLVM_BITCODE_GENERATION_FLAGS="-O1 -Xclang -disable-llvm-passes"

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
echo "copy files..."
    cp -r $FOULDER $build_foulder
    echo "done."
fi

#use flag, so wllvm performs like clang in configuration

if [[ $CMAKE = $true ]] && [[ $MAKE = $false ]]
then
    echo "cmake configure..."
    cd $build_foulder
    wllvm_path=${config[wllvm_path]}/wllvm
    wllvmpp_path=${config[wllvm_path]}/wllvm++
    echo "wllvm path:   " $wllvm_path
    echo "wllvm++ path: " $wllvmpp_path
    echo "clang path:   " $LLVM_COMPILER_PATH
    WLLVM_CONFIGURE_ONLY=1 cmake $FOULDER -G"Unix Makefiles" -DCMAKE_C_COMPILER=$wllvm_path -DCMAKE_CXX_COMPILER=$wllvmpp_path
    MAKE=$true
fi

if [[ $AUTOCONF = $true ]] && [[ $MAKE = $false ]]
then
    echo "autoconf..."
    cd $build_foulder
    WLLVM_CONFIGURE_ONLY=1 autoreconf -i -f
fi

if [[ $CONFIGURE = $true ]] && [[ $MAKE = $false ]]
then
    echo "configure..."
    cd $build_foulder
    WLLVM_CONFIGURE_ONLY=1 ./configure
    MAKE=$true
fi

log_file=$build_foulder/log_$ver.txt

#reset wllvm flag

#build bc-file
if [[ $MAKE = $true ]]
then
    echo "make build..."
    cd $build_foulder
    make -j $(nproc) 2>&1 | tee $log_file
    echo "done."
fi

#generate bc from builded files
filebase=${config[bc_foulder]}/$basename/$ver
if [ ! -d $filebase ]
then 
    mkdir -p $filebase
fi

while read line
do
    matchregex="\[.*\][ ]Linking[ ].*[ ]" #[ ]Linking[ ].*(executable|library)[].*"
    match_executable="executable[ ]"
    match_static="static[ ]"
    match_shared="shared[ ]"
    match_lib="library[ ]"
    match_module="module[ ]"

    if [[  $line =~  $matchregex ]]
    then
        info=$(echo $line | cut -d ' ' -f 4-)
        target=""

        if   [[ $info =~ $match_executable ]] #executable
        then

            target=$(echo $info | cut -d ' ' -f 2-)

            while [[ $target =~ $match_executable ]]
            do
                target=$(echo $target| cut -d ' ' -f 2-)
            done

        elif [[ $info =~ $match_shared ]] || [[ $info =~ $match_static ]] #shared lib
        then

            target=$(echo $info | cut -d ' ' -f 2-)

            while [[ $target =~ $match_module ]] || [[ $target =~ $match_lib ]]
            do
                target=$(echo $target | cut -d ' ' -f 2-)
            done

        fi
        #set type matching to caseinsensitiv
        shopt -s nocasematch

        if [[ -z $target ]] || [[ $target =~ "test" ]]
        then
            continue
        fi 
        #unset type matching
        shopt -u nocasematch

        #remove folder structor from target
        target=$(basename $target)

        target_path=$(find $build_foulder -name $target -type f )
        relativ_path=${target_path/$build_foulder/$filebase}

        ${config[wllvm_path]}/extract-bc -b $target_path
        filename=$(basename $target_path)

        cp "${target_path}.bc" "${filebase}/${filename}.bc"
        echo "copied ${filename}.bc"       
    fi
done < $log_file

done
#           version - loop            #
 ####                             ####
     #############################
else
    echo "skip generation, because foulder(s) wasn't defined"
fi