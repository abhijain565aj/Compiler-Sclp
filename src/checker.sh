make

Device="arch"
out=$(lscpu | grep -c Apple)
if [ $out = 1 ]; then
    Device="mac"
fi

testcase_directory="../example-programs/"
# testcase_directory="../example-programs/Level-5-test-cases/"
# testcase_directry="../example-programs/Level-3-invalid-test-cases/"

testcases=$(find $testcase_directory -name "*.c" | sort)

diff_command="diff -Bw"

green="\e[32m\u2714\e[0m "
red="\e[31m\u2718\e[0m "

#comment the options not to check
opt_checks=()
opt_checks+=("--sa-scan --show-tokens")
opt_checks+=("--sa-parse --show-tokens")
opt_checks+=("--sa-ast --show-ast")
opt_checks+=("--sa-tac --show-tac")
opt_checks+=("--sa-rtl --show-rtl --suppress-comments")
opt_checks+=("--show-symtab --suppress-comments")
opt_checks+=("--show-asm --suppress-comments")
opt_checks+=("--show-tokens --show-ast --show-tac --show-rtl --show-asm --suppress-comments")

#add -v flag for verbose
DIFF_OUTPUT=">/dev/null"
VERBOSE=false
SHOW_ALL=true
COMPLETE_CHECK=false
while [[ "$#" -gt 0 ]]; do
    case $1 in
        -v|--verbose)
            DIFF_OUTPUT=""
            VERBOSE=true
            shift
            ;;

        -a|--show-all)
            SHOW_ALL=false
            shift
            ;;

        -c|--complete-check)
            COMPLETE_CHECK=true
            make clean-log
            shift
            ;;
    esac
done

compare_files() {
    if [ -f "$1" ] && [ -f "$2" ]; then
        if eval "$diff_command $2 $1 $DIFF_OUTPUT"; then
            if [ "$SHOW_ALL" = false ]; then
                echo -e $green "No differences found in $1"
            fi
        else
            echo -e $red "Differences found in $1"
        fi    
    elif [ -f "$1" ] && [ $COMPLETE_CHECK = true ]; then
        echo -e $red "File not found: $2"
    fi
    if [ $COMPLETE_CHECK = true ]; then
        rm -f $1
    fi
}

for opt in "${opt_checks[@]}"; do
    echo -e "\n\e[36m\e[1mProcessing option $opt\e[0m\n"

    rm -f test.c.toks test.c.ast test.c.tac test.c.rtl test.c.spim test.c.sym

    for f in $testcases; do
        ../reference-implementations/A5-sclp-$Device $opt $f 2>/dev/null
        ec1=$?
        
        cp $f test.c
        ./sclp $opt test.c 2>/dev/null
        ec2=$?

        if [[ $ec1 -eq 0 && $ec2 -eq 0 ]]; then
            compare_files "$f.toks" "test.c.toks"
            compare_files "$f.ast" "test.c.ast"
            compare_files "$f.tac" "test.c.tac"
            compare_files "$f.rtl" "test.c.rtl"
            compare_files "$f.sym" "test.c.sym"
            compare_files "$f.spim" "test.c.spim"
        elif [[ $ec1 -eq 0 || $ec2 -eq 0 ]]; then
            echo -e $red "Different exit codes in $f: ref $ec1 self $ec2"
            if [ "$VERBOSE" = true ]; then
                echo "Reference Error: $ec1"
                ../reference-implementations/A5-sclp-$Device $opt $f
                echo "Self Error: $ec2"
                ./sclp $opt test.c
            fi
        else
            if [ "$SHOW_ALL" = false ]; then
                echo -e $green "Same exit codes in $f, both error"
            fi
        fi
    done
done