# CHƯƠNG TRÌNH NÉN VÀ GIẢI NÉN FILE SỬ DỤNG BIẾN ĐỔI BURROWS-WHEELER (BWT)
===================================

## Thiết kế chương trình

### Thuật toán

Quá trình nén được chia làm 4 bước, thao tác với mỗi block dung lượng 900KB.

    1. Biến đổi Burrows-Wheeler
    2. Biến đổi Move-to-front (MTF)
    3. Run-length Encoding (RLE)
    4. Nén bằng mã Huffman (huffman coding)

### Biến đổi BWT
Để sinh ra biến đổi BWT của xâu đầu vào, chương trình sử dụng thuật toán SA-IS để sinh ra Suffix Array, và từ suffix array đó tính ra được xâu biến đổi.

Thuật toán SA-IS (Suffix Array - Induced Sorting) là một thuật toán dựa trên thuật toán KA. Có thời gian thực hiện là tuyến tính. Ý tưởng như sau:
    
   1. Suffix thứ `i` của xâu đầu và `T` có thể hia làm hai loại là `S` và `L`
    với: `Suf(T, i)` là `S` nếu `T[i:] < T[i+1:]`, `Suf(T, i)` là `L` nếu `T[i:] > T[i+1:]`. 
    Từ định nghĩa trên, `Suf(T, i)` là `S` nếu và chỉ nếu   `T[i] < T[i+1]` hoặc `T[i] = T[i+1]` và `Suf(T, i+1)` là `S`. Ta thấy nếu    hai suffix có cùng ký tự đâu, suffix loại `L` sẽ đứng trước suffix loại `S`. Suffix cuối cùng `Suf(T, n-1)` được coi là loại `L`. 
    Có thể xác định loại của tất cả các suffix trong thời gian tuyến tính bằng cách duyệt ngược từ cuối xâu.

   2. `Suf(T, i)` được gọi là `LMS` (Left-most S) nếu như `Suf(T, i) (i > 0)` là `S`      và `Suf(T, i-1)` là `L`. Suffix đầu tiên `Suf(T, 0)` không phải là `LMS`.
      Ta thấy nếu như đã sắp xếp được các suffix      loại `LMS` thì cũng sẽ sắp xếp được ngay suffix loại `L` ngay trước nó.
      Từ đó sắp xếp được các suffix loại `S` còn lại. Quá trình này gọi là
      _induced sorting_.

   3. Để sẵp xếp các `LMS`, ta chia xâu đầu vào thành các `LMS substring`, mỗi  `LMS substring` là một xâu con bắt đầu và kết thúc đều là vị trí `LMS` và ở giũa      không có vị trí nào là `LMS` (với ngoại lệ là xâu con từ vị trí `LMS`      cuối cùng cho đến hết cũng được coi là một `LMS substring`). 
   Thứ tự từ   điển của các `LMS substring` được quy định như sau: Nếu hai ký tự của      substring tại mỗi vị trí giống nhau thì so sánh kiểu (`L` hay `S`), kiểu `S` được coi là lớn hơn kiểu      `L`. 
   Do suffix đầu tiên không phải là `LMS` và số `LMS` trong một xâu không vượt quá 1/2      độ dài xâu (do theo định nghĩa thì không có 2 `LMS` suffix liên tiếp      nhau) nến số lượng `LMS substring` không vượt quá 1/2 độ dài xâu.

   4. Các `LMS substring` có thể được sắp xếp (theo thứ tự từ điển) trong thời      gian tuyến tính bằng Induced Sorting: đầu tiên chia các `LMS suffix` vào      bucket tương ứng với ký tự đầu của nó, sau đó suy ra thứ tự của các `L` suffix đứng trước mỗi `LMS suffix` (suffix `L` luôn đứng đầu bucket của nó), cuối cùng là suy ra thứ tự của các suffix `S` (bao gồm cả các `LMS suffix` ban đầu). Sau quá trình này, tất cả các `LMS Suffix` đều được sắp xếp theo sao cho `LMS Substring` bắt đầu từ nó có thứ tự từ điển tăng dần.

   5. Dựa vào kết quá của bước trên, các `LMS substring` được gán nhãn từ 0 dựa theo thứ tự từ điển của nó trong thời gian tuyến tính, hai
      `LMS substring` bằng nhau thì gán nhãn giống nhau. Thay các `LMS substring` trong xâu gốc bằng nhãn của nó, và bỏ đi những phần còn lại  (phần không thuộc `LMS substring` nào), ta được xâu mới `T1` với độ dài  không vượt quá 1 nửa xâu `T`. 
      Sau đó có thể áp dụng thuật toán 1 cách đệ  quy với xâu `T1` này để sẵp xếp các suffix loại `LMS`. Quá trình đệ quy dừng lại khi không có hai `LMS substring` nào có nhãn giống nhau, khi đó thì thứ tự của các `LMS suffix` đã được sắp xếp ở bước trước cũng chính là  thứ tự của các `LMS suffix` trong suffix array cần tìm.

### Run-length Encoding
Sau biến đổi MTF, chuỗi kết quả sẽ có nhiều byte liên tiếp giống nhau, do
    đó có thể áp dụng RLE để làm giảm kích thước. Ở đây ta mã hóa RLE như sau:
    nếu một byte `b` được lặp lại nhiều hơn 2 lần thì được mã hóa thành `bbbn`
    với n (kích thước 1 byte) là số lần lặp lại `b`, nếu chỉ xuất hiện 1 lần
    thì mã hóa thành `b`, nếu chỉ xuất hiện 2 lần thì mã hóa thành `bb`.
Sau mã hóa RLE, xâu kết quả gồm hai thành phần: xâu chứa các ký tự và xâu chứa
số lần lặp (run-length). Hai xâu này sẽ được nén một cách riêng biệt bằng thuật
toán Huffman.

### Cấu trúc file nén
File nén kết quá chương trình bao gồm nhiều block kích thước khác nhau mã hóa
cho từng block kích thước 900KB của file đầu vào. Mỗi block có nội dung như
sau:

1. Phần tiêu đề
    1. `rle_len`: kích thước của khối dữ liệu sau mã hóa bằng RLE
    2. `pidx`: index của suffix đầu tiên trong biến đổi BWT
    3. `hlen`: kích thước của block sau khi nén bằng thuật toán Huffman
    4. tiêu đề của thuật toán Huffman

2. Phần nội dung:
    1. hufffman(rle(mtf(bwt(T))))


---
## Chạy chương trình
    
Để nén một file `filename` và tạo ra file nén `filename.bwt`, ta sử dụng lệnh:

    ./bwt c filename.bwt filename

(chú ý: tên file đầu vào đứng sau).

Giải nén file nén trên thành file `filename.ex`, ta sử dụng lệnh:

    ./ac x filename.ex filename.bwt

Để nén dữ liệu thời gian thực (từ đầu vào chuẩn `stdin`), ta sử dụng `-` thay
cho tên file đầu vào. VD: nén thư mục hiện tại, kết hợp với `tar`:

    tar cvf - . | ./bwt c directory.tar.ac -

Chú ý: chỉ có mô hình thích nghi mới nén được dữ liệu thời gian thực. Nếu không
thêm `a` vào cuối câu lệnh thì chương trình cũng sẽ tự động chọn mô hình này.

Để giải nén ra đầu ra chuẩn (`stdout`), ta sử dụng `-` thay cho tên file đầu
ra. VD: giải nén thư mục vừa nén vào thư mục `tmp`, kết hợp với `tar`:

    mkdir tmp
    ./bwt c - directory.tar.ac | tar xvf - -C tmp

----
## Kết quả chạy

Chương trình được chạy thử trên máy tính với CPU Intel Pentium B940 2GHz, RAM 3GiB, tốc độ quay của HDD là 5400rpm. Hệ điều hành Linux 3.16.1, 64bit.

Bảng kết quả sau đầy là kết quả của chạy 4 thuật toán với cùng tập file đầu
vào:
1. BWT+MTF+RLE+Huffman
2. gzip
3. bzip2

| File            | Dung lượng | Thuật toán      | File nén | Tỉ lệ nén | Thời gian nén | Giải nén | K/t MD5 |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| 100000fac.txt   | 473673     | BWT+MTF+RLE+HM  | 188325   | 0.398     | 0.15s         | 0.07s    | OK      |
|                 |            | gzip            | 211398   | 0.446     | 0.09s         | 0.00s    | OK      |
|                 |            | bzip2           | 192843   | 0.407     | 0.07s         | 0.04s    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| E.coli          | 4638690    | BWT+MTF+RLE+HM  | 1190950  | 0.257     | 0.94s         | 0.67s    | OK      |
|                 |            | gzip            | 1341475  | 0.289     | 1.48s         | 0.09s    | OK      |
|                 |            | bzip2           | 1251433  | 0.270     | 0.72s         | 0.41s    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| alice29.txt     | 152089     | BWT+MTF+RLE+HM  | 47082    | 0.310     | 0.05s         | 0.06s    | OK      |
|                 |            | gzip            | 54645    | 0.359     | 0.01s         | 0.00s    | OK      |
|                 |            | bzip2           | 43249    | 0.284     | 0.03s         | 0.01s    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| asyoulik.txt    | 125179     | BWT+MTF+RLE+HM  | 42955    | 0.343     | 0.05s         | 0.04s    | OK      |
|                 |            | gzip            | 49140    | 0.393     | 0.01s         | 0.00s    | OK      |
|                 |            | bzip2           | 39814    | 0.318     | 0.01s         | 0.01s    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| bible.txt       | 4047392    | BWT+MTF+RLE+HM  | 935982   | 0.231     | 0.72s         | 0.50s    | OK      |
|                 |            | gzip            | 1191263  | 0.294     | 0.40s         | 0.06s    | OK      |
|                 |            | bzip2           | 845892   | 0.209     | 0.64s         | 0.30s    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| cp.html         | 24603      | BWT+MTF+RLE+HM  | 8080     | 0.328     | 0.05s         | 0.03s    | OK      |
|                 |            | gzip            | 8118     | 0.330     | 0.00s         | 0.00s    | OK      |
|                 |            | bzip2           | 7713     | 0.313     | 0.00s         | 0.00s    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| fields.c        | 11150      | BWT+MTF+RLE+HM  | 3285     | 0.295     | 0.04s         | 0.03s    | OK      |
|                 |            | gzip            | 3263     | 0.293     | 0.00s         | 0.00s    | OK      |
|                 |            | bzip2           | 3159     | 0.283     | 0.00s         | 0.00s    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| grammar.lsp     | 3721       | BWT+MTF+RLE+HM  | 1413     | 0.380     | 0.03s         | 0.03s    | OK      |
|                 |            | gzip            | 1352     | 0.363     | 0.00s         | 0.00s    | OK      |
|                 |            | bzip2           | 1382     | 0.371     | 0.00s         | 0.00s    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| kennedy.xls     | 1029744    | BWT+MTF+RLE+HM  | 119119   | 0.116     | 0.17s         | 0.13s    | OK      |
|                 |            | gzip            | 206917   | 0.201     | 0.06s         | 0.01s    | OK      |
|                 |            | bzip2           | 129974   | 0.126     | 0.10s         | 0.03s    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| lcet10.txt      | 426754     | BWT+MTF+RLE+HM  | 121258   | 0.284     | 0.09s         | 0.06s    | OK      |
|                 |            | gzip            | 144638   | 0.339     | 0.04s         | 0.00s    | OK      |
|                 |            | bzip2           | 107793   | 0.253     | 0.06s         | 0.03s    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| plrabn12.txt    | 481861     | BWT+MTF+RLE+HM  | 159255   | 0.330     | 0.10s         | 0.07s    | OK      |
|                 |            | gzip            | 195395   | 0.406     | 0.06s         | 0.00s    | OK      |
|                 |            | bzip2           | 145648   | 0.302     | 0.07s         | 0.03s    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| ptt5            | 513216     | BWT+MTF+RLE+HM  | 61946    | 0.121     | 0.07s         | 0.05s    | OK      |
|                 |            | gzip            | 56573    | 0.110     | 0.02s         | 0.00s    | OK      |
|                 |            | bzip2           | 49884    | 0.097     | 0.02s         | 0.01s    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| sum             | 38240      | BWT+MTF+RLE+HM  | 13700    | 0.358     | 0.03s         | 0.03s    | OK      |
|                 |            | gzip            | 12988    | 0.340     | 0.00s         | 0.00s    | OK      |
|                 |            | bzip2           | 13001    | 0.340     | 0.00s         | 0.00s    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| world192.txt    | 2473400    | BWT+MTF+RLE+HM  | 543758   | 0.220     | 0.43s         | 0.31s    | OK      |
|                 |            | gzip            | 724586   | 0.293     | 0.16s         | 0.03s    | OK      |
|                 |            | bzip2           | 489904   | 0.198     | 0.39s         | 0.19s    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|
| xargs.1         | 4227       | BWT+MTF+RLE+HM  | 1887     | 0.446     | 0.04s         | 0.03s    | OK      |
|                 |            | gzip            | 1866     | 0.441     | 0.00s         | 0.00s    | OK      |
|                 |            | bzip2           | 1866     | 0.441     | 0.00s         | 0.00s    | OK      |
|-----------------|------------|-----------------|----------|-----------|---------------|----------|---------|

Bảng kết quả trên được tạo ra bằng cách chạy script `compare.sh` đi kèm:

    ./compare.sh <all test file>

Với `<all test file>` là các file cần nén.
