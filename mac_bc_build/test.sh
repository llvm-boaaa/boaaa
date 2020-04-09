log_file="/Volumes/BOAAA/build/build_40/log_40.txt"

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

        echo $target
        target_path=$(find $build_foulder -name $target)
        relativ_path=${target_path/$build_foulder/$filebase}

        ${config[wllvm_path]}/extract-bc -b $target_path
        filename=$(basename $target_path)

        cp "${target_path}.bc" "${filebase}/${filename}.bc"
        echo "copied ${filename}.bc"       
    fi
done < $log_file