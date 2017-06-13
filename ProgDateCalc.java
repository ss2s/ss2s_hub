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

public void abcSet (double af, double bf, double cf) {

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

public void znakSet (char znak1f, char znak2f) {

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

	} else {

		//cout << "err: a >= maxVal";
	}
}

public int findDay () {

	// функция расчитывает день недели из даты в формате dd/mm/yyyy
	// на выходе int 0-сб 1-вс 2-пн 3-вт 4-ср 5-чт 6-пт

	int dd = 32, mm = 13, yyyy = -1;
	int nyyyy, nmm, iday, day, flag = 0;
	boolean enStart = false; // enable findDay
	boolean enFlagD = false; // validation check of dd
	boolean enFlagM = false; // validation check of mm
	boolean enFlagY = false; // validation check of yyyy
	int fd = 98; // здесь будет значение возвращаемое функцией
	// 1-пн 2-вт 3-ср 4-чт 5-пт... 29,30-ошибка в этом мес. 29,30 дней
	// 28-ошибка(не выс. год 28 дней) 90-ошибка(некoректный ввод)
	// 91-dd==false 92-mm==false 93-yyyy==false 98,99-other error

	fd +=1;
    dd = a;
    mm = b;
    yyyy = c;

    if (dd <= 31 && dd > 0) {

    	enFlagD = true;
    }

    if (mm <= 12 && mm > 0) {

       	enFlagM = true;
    }

    if (yyyy > 0) {

    	enFlagY = true;
    }

    if (enFlagD == true && enFlagM == true && enFlagY == true) {

    	enStart = true;

    } else if (enFlagD == false && enFlagM == true && enFlagY == true) {

    	enStart = false;
    	fd = 91;
    	return fd;

    } else if (enFlagD == true && enFlagM == false && enFlagY == true) {

    	enStart = false;
    	fd = 92;
    	return fd;

    } else if (enFlagD == true && enFlagM == true && enFlagY == false) {

    	enStart = false;
    	fd = 93;
    	return fd;

    } else {

    	enStart = false;
    	fd = 90;
    	return fd;
    }

    switch (mm) {

    	case 2:

    		if ((yyyy % 400) == 0 || ((yyyy % 4) == 0 && (yyyy % 100) != 0)) {

    			if (dd > 29) {

    				enStart = false;
    				fd = 29;
    				return fd;
    			}

    		} else {

    			if (dd > 28) {

    				enStart = false;
    				fd = 28;
    				return fd;
    			}
    		}

    		break;

    	case 4:

    		if (dd > 30) {

    			enStart = false;
    			fd = 30;
    			return fd;
    		}

    		break;

    	case 6:

    		if (dd > 30) {

    			enStart = false;
    			fd = 30;
    			return fd;
    		}

    		break;

    	case 9:

    		if (dd > 30) {

    			enStart = false;
    			fd = 30;
    			return fd;
    		}

    		break;

    	case 11:

    		if (dd > 30) {

    			enStart = false;
    			fd = 30;
    			return fd;
    		}

    		break;
    }

    if (enStart == true) {

    	if (mm <= 2) {

    		nyyyy = yyyy - 1;
    		nmm = 0;
    	}
    	else {

    		nyyyy = yyyy;
    		nmm = (4 * mm + 23) / 10;
    	}

    	iday = 365 * yyyy + dd + 31 * (mm - 1) - nmm + (nyyyy / 4) - ((3 * ((nyyyy / 100) + 1) / 4));
    	day = iday % 7;

    	switch (day) {

    		case 0:
    			fd = 6;
    			break;

    		case 1:
    			fd = 7;
    			break;

    		case 2:
    			fd = 1;
    			break;

    		case 3:
    			fd = 2;
    			break;

    		case 4:
    			fd = 3;
    			break;

    		case 5:
    			fd = 4;
    			break;

    		case 6:
    			fd = 5;
    			break;

    	}



    }

    return fd;

}

public double startCalc () {}

}
