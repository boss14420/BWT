#!/bin/bash - 
#===============================================================================
#
#          FILE: compare.sh 
# 
#         USAGE: ./compare.sh [file_list]
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: BOSS14420 (), 
#  ORGANIZATION: 
#       CREATED: 09/09/2014 08:34
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

BWT_HOME="$(dirname $0)"
BWT="$BWT_HOME/bwt"
TMP="/tmp/bwt"

hline(){
    python2 -c "print '|' + '-'*17 + '|' + '-'*12 + '|' + '-'*17 +'|' + '-'*10 + \
                '|' + '-'*11 + '|' + '-'*15 + '|' + '-'*10 + '|' + '-'*9 + '|'"
}

compress_test(){
    file="$1"
    prog="$2"
    copt="$3"
    xopt="$4"
    ext="$5"
    xext="$6"

    filename=$(basename "$file")
    cfile="$TMP/$filename.$ext"
    if [[ -n "$xext" ]]; then
        xfile="$TMP/$filename.$xext"
    else
        xfile="$TMP/$filename"
    fi


    if [[ $prog = "tar" ]]; then
        dir=$(dirname "$file")
        ctime=$(/usr/bin/time -f "%e" "$prog" "$copt" "$cfile" -C "$dir" "$filename" 2>&1 >/dev/null)
    else
        ctime=$(/usr/bin/time -f "%e" "$prog" "$copt" "$cfile" "$file" 2>&1 >/dev/null)
    fi
    hcsz=$(/bin/ls -lh "$cfile" | cut -d" " -f5)iB
    sz=$(stat --printf "%s" "$file")
    csz=$(stat --printf "%s" "$cfile")
    ratio=$(echo "$csz / $sz" | bc -l)
    ratio=$(printf "%.03f" $ratio)
    printf " %-8s | %-09s | %-13s |" $hcsz $ratio "$ctime"s

    if [[ $prog = "tar" ]]; then
        xtime=$(/usr/bin/time -f "%e" "$prog" "$xopt" "$cfile" -C "$TMP" 2>&1 >/dev/null)
    else
        xtime=$(/usr/bin/time -f "%e" "$prog" "$xopt" "$xfile" "$cfile" 2>&1 >/dev/null)
    fi
    printf " %-8s |" "$xtime"s

    md51=$(md5sum "$file" | cut -d" " -f1)
    md52=$(md5sum "$xfile" | cut -d" " -f1)
    if [[ $md51 != $md52 ]]; then
        printf " %-9s |" "Lỗi"
    else
        printf " %-7s |" "OK"
        rm -rf "$cfile" "$xfile"
    fi
    printf "\n"
}

hline
printf "| %-15s | %-10s | %-18s | %-8s | %-9s | %-13s | %-8s | %-6s |\n" \
    "File" "Dung lượng" "Thuật toán" "File nén" "Tỉ lệ nén" "Thời gian nén" "Giải nén" "K/t MD5"
hline

mkdir -p "$TMP" &> /dev/null

for file in "$@"
do
    filename=$(basename "$file")
    hsz=$(/bin/ls -lh "$file" | cut -d" " -f5)iB
    printf "| %-15s | %-10s |" "$filename" $hsz

    ## BWT + MTF + RLE + HM
    algo="BWT+MTF+RLE+HM"
    printf " %-15s |" "$algo"
    compress_test "$file" "$BWT" "c" "x" "bwt" "bwtx"

    ## Gzip
    algo="gzip"
    printf "|%-17s|%-12s| %-15s |" "" "" "$algo"
    compress_test "$file" "tar" "czf" "xzf" "bz2" ""

    ## Bzip2
    algo="bzip2"
    printf "|%-17s|%-12s| %-15s |" "" "" "$algo"
    compress_test "$file" "tar" "cjf" "xjf" "bz2" ""

    hline
done

rmdir --ignore-fail-on-non-empty "$TMP"

#hline
