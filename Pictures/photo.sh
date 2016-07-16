#!/bin/bash
#создаем массив фотографий по маске, потом сканируем каждую на qr код, удаляем если не нашли
1>log.txt #стираем прошлую историю(?)
array=(`ls *.jpg`)
len=${#array[*]}
i=0
while [ $i -lt $len ]; do
#echo "$i: ${array[$i]}"
 out=$(zbarimg ${array[$i]})
 if [ ! -z "$out" ]; # если не nill
 then
  echo "${array[$i]} | out-decode | $out" 1>>log.txt #добавление в лог
 else
  $(rm ${array[$i]}) #удаление 
 fi
 let i++
done

#везде одинаковый qr-код
