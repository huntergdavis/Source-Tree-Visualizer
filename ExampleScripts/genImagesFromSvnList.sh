#/bin/bash

#// loop through a file, item1= url, item2=name
cd ~/Projects/source_tree_vis/
touch ./out/start.xml
echo "<xml><simple_tree_options><print_mode>file</print_mode></simple_tree_options></xml>" > ./out/start.xml
ITERATOR=1;
PREVNAME="start"
NAME="noname"
REPO="norepo"
for i in $(cat ./ExampleScripts/svnlist.txt); 
do 
	let ITERATOR="($ITERATOR + 1)";
	let evencheck="($ITERATOR%2)"
	if [ $evencheck -eq 0  ] 
	then
   	REPO="$i";
	else
   	NAME="$i";
	./bin/simple_tree -S $REPO -o $NAME.png -H $NAME.html -x $NAME.xml -B $NAME.debuglog -f $NAME.log -c ./out/$PREVNAME.xml
	PREVNAME=$NAME;
	fi
done

#./bin/simple_tree -S http://miranda.googlecode.com/svn/trunk/  -o miranda-im.png && ristretto ./out/*.png
