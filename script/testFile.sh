#!/bin/bash
virusFile=$1
countriesFile=$2
numLines=$3
Duplicates=$4

if [[ "$#" -ne 4 ]]; then
	echo "Scrip requires exactly 4 command line arguments(./testFile.sh [virusFile] [countriesFile] [numLines] [Duplicates 0 or 1]"
	exit 1
fi

if [[ "$numLines"<1 ]]; then
	echo"number of lines must be >=1 "
	exit 1
fi

if [[ "$Duplicates" -ne 1 ]] && [[ "$Duplicates" -ne 0 ]]; then
	echo "Duplicate must be either 1 or 0 "
	exit 1
fi
#echo >inputfile.txt

charl=abcdefghijklmnopqrsrtvuwyz
charU=ABCDEFGHIJKLMNOPQRSRTVUWYZ
index="0"
entries=($(shuf -i 0-9999 -n 10000))

if [ -f inputfile.txt ]; then
   rm inputfile.txt
fi

for (( i = 0; i < $numLines; i++ )); do

	if (($Duplicates == 1 && $i>2 && ($RANDOM %2 == 0 || $index >9999) )); then
		line=($(shuf -n 1 inputfile.txt))
		citizenID=${line[0]}
		firstName=${line[1]}
		lastName=${line[2]}
		country=${line[3]}
		age=${line[4]}
	else
		if (( $index >9999 )); then
			echo "exeeded 10000 records and Duplicates are not allowed"
			exit 1
		fi
		citizenID="0"
		citizenID=${entries[$index]}
		index=$[$index+1]

		firstName="${charU:RANDOM%${#charU}:1}"
		max=$(($RANDOM%10+1))

		for (( j = 0; j < max; j++ )); do
			firstName="$firstName${charl:RANDOM%${#charl}:1}"
		done

		lastName="${charU:RANDOM%${#charU}:1}"
		max=$(($RANDOM%10+1))
		for (( j = 0; j < max; j++ )); do
			lastName="$lastName${charl:RANDOM%${#charl}:1}"
		done

		country=$(shuf -n 1 "$countriesFile")

		age=$(($RANDOM%120+1))
	fi


	virusName=$(shuf -n 1 "$virusFile")

	if [[ "$RANDOM % 2" -eq 1 ]]; then
		Y_N="YES"
		date="$(($RANDOM%29+1))-$(($RANDOM%11+1))-$(($RANDOM%71+1950))"
		#dateVaccinated=
	else
		Y_N="NO"
		date=""
	fi
	#echo "${entries[$index]}"
	#echo "$citizenID $firstName $lastName $country $age $virusName $Y_N $date"
	echo "$citizenID $firstName $lastName $country $age $virusName $Y_N $date" >> inputfile.txt

done
echo "done"
#echo ${unselected[@]}

exit 0