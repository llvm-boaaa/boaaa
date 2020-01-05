function(boaaa_replace_last COM REP OUTPUT_ INPUT_)
	string(REPLACE "${COM}" ";" __list ${INPUT_})
	list(LENGTH __list __len)
	math(EXPR __len2 "${__len}-1") # last index
	math(EXPR __len "${__len}-2") # len "a;b;c" = 3 -> RANGE 0 1(3-2), every except last
	set(${OUTPUT_} "")
	foreach(__i RANGE 0 ${__len})
	list(GET __list ${__i} __tmp)
	set(${OUTPUT_} "${${OUTPUT_}}${__tmp}")
	endforeach(__i)
	set(${OUTPUT_} "${${OUTPUT_}}${REP}")
	list(GET __list ${__len2} __tmp)
	set(${OUTPUT_} ${${OUTPUT_}}${__tmp})
	message(STATUS "||test|| ${OUTPUT_} :=: ${${OUTPUT_}}")
	set(${OUTPUT_} ${${OUTPUT_}} PARENT_SCOPE) #return
endfunction()