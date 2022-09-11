#include <iostream>
#include <vector>
#include <ctime>
using namespace std;

unsigned long long modularPow(unsigned long long b, unsigned long long p,
		unsigned long long m) {
	if (p == 0)
		return 1;
	if (p == 1)
		return b % m;
	if (!(p % 2))
		return modularPow(b * b % m, p / 2, m) % m;
	return b * modularPow(b * b % m, (p - 1) / 2, m) % m;
}

unsigned long long gcd(unsigned long long a, unsigned long long b) {
	if (a == 1 || b == 1)
		return 1;
	if (a % b == 0)
		return b;
	if (b % a == 0)
		return a;
	return (a > b) ? gcd(b, a % b) : gcd(a, b % a);
}

unsigned long long mod_inverse(unsigned long long a, unsigned long long m) {
	unsigned long long m0 { m }, t { }, q { };
	unsigned long long x0 { 0 }, x1 { 1 };
	if (m == 1)
		return 1;
	while (a > 1) {
		q = a / m;
		t = m;
		m = a % m;
		a = t;
		t = x0;
		x0 = x1 - q * x0;
		x1 = t;
	}
	if (x1 < 0)
		x1 += m0;
	return x1;
}

/*
 * Класс ключей
 */

class Key {
	static const vector<unsigned long long> primes;
	pair<unsigned long long, unsigned long long> private_key;
	pair<unsigned long long, unsigned long long> public_key;

	void makeKeyPair() {

		unsigned long long n1 { primes[rand() % (primes.size() + 1)] }, n2 {
				primes[rand() % (primes.size() + 1)] };
		unsigned long long N { n1 * n2 }, Phi { (n1 - 1) * (n2 - 1) };
		unsigned long long E { };
		do {
			E = 3 + rand() % (Phi - 2);
		} while (gcd(E, Phi) != 1);
		unsigned long long D { mod_inverse(E, Phi) };
		this->private_key = make_pair(E, N);
		this->public_key = make_pair(D, N);
	}
public:
	Key() {
		this->makeKeyPair();
	}
	pair<unsigned long long, unsigned long long> getPrivate() {
		return this->private_key;
	}
	pair<unsigned long long, unsigned long long> getPublic() {
		return this->public_key;
	}
};

const vector<unsigned long long> Key::primes { 283, 293, 307, 311, 313, 317,
		331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409,
		419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491,
		499, 503, 509, 521, 523, 541, 547, 557, 563, 569, 571, 577, 587, 593,
		599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 661, 673,
		677, 683, 691, 701, 709, 719, 727, 733, 739, 743, 751, 757, 761, 769,
		773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863,
		877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 947, 953, 967, 971,
		977, 983, 991, 997 };

class Client {
	int id;
public:
	Client() {
		this->id = rand();
	}
	int getId() {
		return this->id;
	}
};

class DialogImplementation {
protected:
	Key *key;
public:
	virtual vector<unsigned long long> encrypt(string message)=0;
	virtual string decrypt(vector<unsigned long long> crypt)=0;
	virtual ~DialogImplementation() {
	}
};

class DialogImplementationRSA: public DialogImplementation {
public:
	DialogImplementationRSA(Key *key) {
		this->key = key;
	}

	vector<unsigned long long> encrypt(string message) {
		vector<unsigned long long> ret;
		unsigned long long E(this->key->getPublic().first);
		unsigned long long N(this->key->getPublic().second);
		for (auto c : message) {
			unsigned long long Crypt = modularPow((unsigned long long) c, E, N);
			//cout << Crypt << endl;
			ret.push_back(Crypt);
		}
		return ret;
	}

	string decrypt(vector<unsigned long long> crypt) {
		string ret = "";
		unsigned long long D(this->key->getPrivate().first);
		unsigned long long N(this->key->getPrivate().second);
		for (auto Crypt : crypt) {
			char c = (char) modularPow(Crypt, D, N);
			//cout << c << endl;
			ret += c;
		}
		return ret;
	}
};

class DialogAbstraction {
	Client *alice;
	Client *bob;
	DialogImplementation *implementation;
public:
	DialogAbstraction(Client *alice, Client *bob,
			DialogImplementation *implementation) :
			alice(alice), bob(bob), implementation(implementation) {
	}
	vector<unsigned long long> send(string message, bool AliceToBob) {
		if (AliceToBob)
			cout << this->alice->getId() << " sent a message to "
					<< this->bob->getId() << endl;
		else
			cout << this->bob->getId() << " sent a message to "
					<< this->alice->getId() << endl;
		return this->implementation->encrypt(message);
	}

	string receive(vector<unsigned long long> message, bool AliceToBob) {
		if (AliceToBob)
			cout << this->bob->getId() << " received a message from "
					<< this->alice->getId() << endl;
		else
			cout << this->alice->getId() << " received a message from "
					<< this->bob->getId() << endl;
		return this->implementation->decrypt(message);
	}

};

class DialogSecurityProxy {
	DialogAbstraction *dialog_abstraction;
public:
	DialogSecurityProxy(DialogAbstraction *dialog_abstraction) :
			dialog_abstraction(dialog_abstraction) {
	}
	void sendMessage(string message, bool AliceToBob = 0) {
		string msg = this->dialog_abstraction->receive(
				this->dialog_abstraction->send(message, AliceToBob),
				AliceToBob);
		cout << "Message: " << endl << msg << endl;
	}
};

int main() {
	srand(time(NULL));
	//cout << primes[rand() % (primes.size() + 1)] << endl;
	//cout << makeKeyPair() << endl;
//	Key key;
//	cout << "Private key: " << key.getPrivate().first << ", "
//			<< key.getPrivate().second << endl;
//	cout << "Public key: " << key.getPublic().first << ", "
//			<< key.getPublic().second << endl;
//	cout << gcd(6, 4) << endl;
//	cout << gcd(66, 48) << endl;
//	cout << gcd(16, 45) << endl;
//	cout << mod_inverse(9, 11) << endl;
//	string test = "";
//	test += 'H';
//	test += 'i';
//	cout << "Test " << test << endl;
//	cout << modularPow(2, 8, 253) << endl;
	DialogSecurityProxy dsp(
			new DialogAbstraction(new Client(), new Client(),
					new DialogImplementationRSA(new Key)));
	dsp.sendMessage("Hello!", 1);
	return 0;
}
