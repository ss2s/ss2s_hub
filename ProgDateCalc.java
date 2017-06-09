class ProgDateCalc {

	int z1 = 0, z2 = 0;
	char znak1 = '^', znak2 = '^';
	double a, b, c;
	int counter = 1, n = 1;
	long maxVal = 562949953421312 * n;

public ProgDateCalc () {
a = 2;
b = 2;
c = 0;
}

public void nSet (int nf) {
	this.n = nf;
}

public void numSet (double af,bf,cf) {
	this.a = af;
	this.b = bf;
	this.c = cf;
}

public void charSet (char znak1f,znak2f) {
	this.znak1 = znak1f;
	this.znak2 = znak2f;
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

public int findDay () {}

public int startCalc () {}

}
