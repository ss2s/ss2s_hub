class ProgDateCalc {

	int z1 = 0, z2 = 0;
	char znak1 = '^', znak2 = '^';
	double a = 0, b = 0, c = 0, d = 0;
	int counter = 1, n = 1;
	long maxVal = 562949953421312 * n;

public ProgDateCalc () {
a = 0;
b = 0;
c = 0;
}

public ProgDateCalc () {}

public void nSet (int nf) {

	this.n = nf;
}

public void abcSet (double af,bf,cf) {

	this.a = af;
	this.b = bf;
	this.c = cf;
}

public void aSet (double af) {

	this.a = af;
}

public void bSet (double bf) {

	this.b = bf;
}

public void cSet (double cf) {

	this.c = cf;
}

public void znakSet (char znak1f,znak2f) {

	this.znak1 = znak1f;
	this.znak2 = znak2f;
	this.z1 = znak1f;
	this.z2 = znak2f;
}

public void znak1Set (char znak1f) {

	this.znak1 = znak1f;
	this.z1 = znak1f;
}

public void znak2Set (char znak2f) {

	this.znak2 = znak2f;
	this.z2 = znak2f;
}

public void startProg () {

	if (a <= maxVal) {
		while (a <= maxVal) {
			//cout << "N "<<counter<<".     "<<a<<endl;
			a = a * b;
			counter ++;
		}
	}else{
		//cout << "err: a >= maxVal";
	}
}

public int findDay () {

	int dd = 32, mm = 13, yyyy = -1;
	int nyyyy, nmm, iday, day, flag = 0;
	boolean enFlag = false; // enable findDay
	int fd = 10; // здесь будет значение возвращаемое функцией
	// 0-сб 1-вс 2-пн 3-вт 4-ср 5-чт 6-пт
	// 28-ошибка(невысокосный год) 90-ошибка(некoректный ввод)
	// 91-dd==false 92-mm==false 93-yyyy==false 99-other error

    dd = a;
    mm = b;
    yyyy = c;

    if ()


}

public double startCalc () {}

}
