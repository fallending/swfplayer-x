<? 
$scale=20;
Ming_setScale($scale);
ming_useswfversion(6);

$movie=new SWFMovie();
$width=800; $height=600;
$movie->setDimension($width,$height);
$movie->setRate(30);

$movie->add(new SWFAction("
	function pad(str2) {
		return str2;
	}

	table = new Array();
	table[0]=\"0\";
	table[1]=(\"13\");
	table[2]=(false);
	table[3]=(Math.sin);
	table[4]=(pad);
	table[5]=(undefined);
	table[6]=(null);

	// Because the order is not preserved,
	// we should check this way
	str='';
	for(i in table) {
		str+='|'+i+','+table[i]+'|';
	}

	str2='';
	for(j=0;j<7;++j) {
		str_temp=('|'+j+','+table[j]+'|');
		if (str.indexOf(str_temp)<0) {
			trace(j+'('+table[j]+') not found');
		}
		str2+='|'+table[j]+'|';
	}
	trace(str2.Length()==str.Length());
"));
$movie->nextframe();

$movie->save("test_forin_array.swf");
?>
