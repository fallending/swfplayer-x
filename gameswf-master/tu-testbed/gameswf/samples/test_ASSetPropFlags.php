<? 
$scale=20;
Ming_setScale($scale);
ming_useswfversion(6);

$movie=new SWFMovie();
$width=800; $height=600;
$movie->setDimension($width,$height);
$movie->setRate(30);

$props=array();
$props[]='null';
$props[]='[\'0\']';
$props[]='[\'0\', \'1\']';

$action = "
{
	var table1 = new Array('a1','b1','c1');
//table1['push']='toto';
//var m = table1.push('d1','e1');
//trace('Longueur =' + m);
//trace('table1 =' + table1);

str='';
for( i in table1) {
	str+='|'+i+'|';
}

val=0;
// Because for the 'for .. in ..' instruction, the order of the result
// is not important, and the batch test want the same strict input,
// we use this (not perfect) code:
for( j=0;j<str.length;++j) {
	val+=str.charCodeAt(j);
}
trace(val);
}

";

for ($b =0; $b <2;++$b) {
	for ($j = 0; $j<8;++$j) {
		foreach ($props as $i) {
			$action = $action .
			"
{
	var table1 = new Array('a1','b1','c1');
			ASSetPropFlags(table1, $i , $j, ".(($b&1)?"1":"0").");

			trace(\"******** $i - $j - ".(($b&1)?"1":"0")." ******\");

			str='';
			for( i in table1) {
				str+='|'+i+'|';
			}

			val=0;
			// Because for the 'for .. in ..' instruction, the order of the result
			// is not important, and the batch test want the same strict input,
			// we use this (not perfect) code:

			for( j=0;j<str.length;++j) {
				val+=str.charCodeAt(j);
			}
			trace(val);
		}
		";
	}
}
}

for ($b =0; $b <2;++$b) {
	for ($j = 0; $j<8;++$j) {
		foreach ($props as $i) {
			$action = $action .
			"
{
	var table1 = new Array('a1','b1','c1');
			ASSetPropFlags(table1, $i , $j, ".(($b&1)?"1":"0").");

			trace(\"******** $i - $j - ".(($b&1)?"1":"0")." ******\");

			table1[0]='a2';
			table1[1]='b2';
			table1[2]='c2';
			
			str='';
			for( i=0; i<3;++i) {
				str+='|'+table1[i]+'|';
			}

			trace(str);
		}
		";
	}
}
}

$action = $action."
trace('*************************');
//create a new object
function test() {
};

myproperties = new test();
//place a few properties inside
myproperties.firstname = \"Guy\";
myproperties.surname = \"Watson\";
myproperties.icq = 71063418;

//define the function that iterates
//over all the children of the myproperties object
function doIterate() {
	var table = [\"firstname\",\"surname\", \"icq\"];
	var result = new Array( null, null, null);

	for (i in myproperties) {
		for(j=0;j<3;++j) {
			if (i == table[j]) {
				result[j] = i;
				break;
			}
		}
		if (j==3)
	       	{
			trace(i);
		}
	}

	for(j=0;j<3;++j) {
		if (result[j] != null) {
			trace(result[j]);
		}
	}
}
//call the function
doIterate();

test=0;

trace('************************* '+ (++test));
ASSetPropFlags(myproperties,[\"firstname\",\"surname\"],6,1);
doIterate();

trace('************************* '+ (++test));
ASSetPropFlags(myproperties,null,6,1);
doIterate();

trace('************************* '+ (++test));
ASSetPropFlags(myproperties,[\"icq\"],0,1);
doIterate();

trace('************************* '+ (++test));
ASSetPropFlags(myproperties,null,0,1);
doIterate();

trace('************************* '+ (++test));
ASSetPropFlags(myproperties,[\"firstname\"],2,1);
delete myproperties.firstname;
trace(myproperties.firstname);

trace('************************* '+ (++test));
ASSetPropFlags(myproperties,[\"firstname\"],0,1);
delete myproperties.firstname;
trace(myproperties.firstname);

trace('************************* '+ (++test));
myproperties.firstname=\"Guy\";

trace('************************* '+ (++test));
ASSetPropFlags(myproperties,null,2,1);
delete myproperties.firstname;
delete myproperties.surname;
delete myproperties.icq;

trace('************************* '+ (++test));
trace(myproperties.firstname);
trace(myproperties.surname);
trace(myproperties.icq);

trace('************************* '+ (++test));
ASSetPropFlags(myproperties,null,0,1);
delete myproperties.firstname;
delete myproperties.surname;
delete myproperties.icq;

trace(myproperties.firstname);
trace(myproperties.surname);
trace(myproperties.icq);

trace('************************* '+ (++test));
myproperties.firstname=\"Guy\";
myproperties.surname=\"Watson\";
myproperties.icq=71063418;

ASSetPropFlags(myproperties,[\"firstname\"],4,1);
myproperties.firstname=\"Richard\";
trace(myproperties.firstname);

trace('************************* '+ (++test));
ASSetPropFlags(myproperties,[\"firstname\"],0,1);
myproperties.firstname=\"Richard\";
trace(myproperties.firstname);

trace('************************* '+ (++test));
myproperties.firstname=\"Guy\";
ASSetPropFlags(myproperties,null,4,1);

myproperties.firstname=\"Richard\";
myproperties.surname=\"Blackwood\";
myproperties.icq=71063418;

trace(myproperties.firstname);
trace(myproperties.surname);
trace(myproperties.icq);
trace('*************************');
";

print $action;

$movie->add(new SWFAction($action));
$movie->nextframe();

$movie->save("test_ASSetPropFlags.swf");
?>
