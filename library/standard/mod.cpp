#pragma once
#include "../basic.cpp"

size_t Dynamic_Mod_P=100;

class DynamicMod{
	public:
		size_t val;
		DynamicMod(){val=0;}
		DynamicMod(size_t init):val((init<Dynamic_Mod_P)?init:init%Dynamic_Mod_P){}
		inline DynamicMod operator+(DynamicMod other){return DynamicMod(val+other.val);}
		inline DynamicMod operator-(DynamicMod other){return DynamicMod(Dynamic_Mod_P+val-other.val);}
		inline DynamicMod operator*(DynamicMod other){return DynamicMod(val*other.val);}
		size_t Value(){return val;}
		bool operator == (DynamicMod other){return (val==other.val);}
		DynamicMod& operator =(DynamicMod other){val=other.val;return *(this);}
};


template <size_t P> class Mod {//P should be less than root MAX_SIZE_N
	
	
	size_t val;
	Mod(size_t init):val(init){}
		
	public:
		constexpr static bool IsMont=(P==998244353);
		//constexpr static bool IsMont=false;
		constexpr static size_t LogR=(IsMont?(size_t)30:0);
		constexpr static size_t R_dash=928055296;
		constexpr static size_t P_dash=998244351;
		constexpr static size_t ModR=((size_t)1<<LogR)-1;
		
		
		Mod(){val=0;}
		
		inline size_t Montgomery(size_t T){
			size_t res=(((((T&ModR)*P_dash)&ModR)*P+T)>>LogR);
			if(res>=P) res-=P;
			return res;
		}
		
		
		size_t convert(signed long long int n){return static_cast<size_t>(static_cast<signed long long int>(P)+(n%static_cast<signed long long int>(P)))%P;} 
		inline Mod& Set(size_t init){
			if constexpr (IsMont){
				if(init==0) val=init;
				else{
					init=init<P?init:init%P;
					init<<=LogR;
					val=init%P;
				}
			}else{
				val=init<P?init:init%P;
			}
			return *this;
		}
		inline Mod& Set(int init){return Set(convert(static_cast<signed long long int>(init)));}
		inline Mod& Set(double init){return Set(convert(static_cast<signed long long int>(std::round(init))));}
		inline Mod& Set(signed long long int init){return Set(convert(init));}
		
		
		
		inline Mod operator+(Mod other){
			size_t res=val+other.val;
			if(res>P){
				return Mod(res-P);				
			}
			return Mod(res);
		}
		inline Mod& operator += (Mod other){
			val+=other.val;
			if(val>=P) val-=P;
			return *(this);
		}
		inline Mod operator-(Mod other){
			size_t res=P+val-other.val;
			if(res>=P){
				return Mod(res-P);				
			}
			return Mod(res);
		}
		inline Mod& operator -= (Mod other){
			val+=P;
			val-=other.val;
			if(val>=P) val-=P;
			return *(this);
		}
		inline Mod operator*(Mod other){
			if constexpr (IsMont) {
				return Mod(Montgomery(val*other.val));
			}else{
				size_t res=val*other.val;
				if(res>=P) res=res%P;
				return Mod(res);
			}
		}
		inline Mod operator/(Mod other){return (*this)*other.Inv();}
	
		inline Mod& operator *= (Mod other){
			if constexpr (IsMont) {
				val=Montgomery(val*other.val);
			}else{
				val*=other.val;
				if(val>=P) val=val%P;
			}
			return (*this);
		}
		inline Mod& operator /= (Mod other){
			if constexpr (IsMont) {
				val=Montgomery(val*(other.Inv().val));
			}else{
				val*=other.Inv().val;
				if(val>=P) val=val%P;
			}
			return (*this);
		}
		inline Mod Inv(){
			if(val==0) std::exit(EXIT_FAILURE);
			if constexpr (IsMont){
				return Mod(static_cast<size_t>(static_cast<signed long long int>(P)+((AxBy(static_cast<signed long long int>(Montgomery(Montgomery(val))),static_cast<signed long long int>(P)))[0])));
			}else{
				return Mod(static_cast<size_t>(static_cast<signed long long int>(P)+((AxBy(static_cast<signed long long int>(val),static_cast<signed long long int>(P)))[0])));
			}
		}
		
		
		bool operator ==(Mod other){return val==other.val;}
		size_t Value(){
			if constexpr (IsMont){
				return Montgomery(this->val);
			}else{
				return val;
			}
		}
		Mod& operator=(size_t num){
			return Set(num);
		}
		Mod& operator=(int num){
			return Set(num);
		}
		
		Mod& operator=(double num){
			return Set(num);
		}
		
};







template <size_t  P>
std::ostream& operator << (std::ostream& os, Mod<P> val){
	os<<val.Value();
	return os;
}

template <ull P>
class OneValue<Mod<P>> {
	public:
		constexpr static Mod<P> value=Mod<P>().Set(1);
};



template <size_t P>
inline LightArray<Mod<P>> fouriertransform_freq(Array<Mod<P>>& arr,size_t degree,Mod<P> zeta){
	size_t length=1<<degree;
	
	
	Mod<P>** res=new (std::nothrow) Mod<P>*[degree+1];
	if(res==nullptr) std::exit(EXIT_FAILURE);
	for(size_t i=1;i<=degree;++i){
		res[i]=new (std::nothrow) Mod<P>[length];
		if(res[i]==nullptr) std::exit(EXIT_FAILURE);
	}
	
	Mod<P> tempzeta;
	tempzeta=1;
	LightArray<Mod<P>> cache;
	cache.Allocate(length>>1);
	{
		size_t loc1=0;
		size_t loc2=length>>1;
		
		while(loc2<length){
			res[1][loc1]=arr[loc1]+arr[loc2];
			res[1][loc2]=tempzeta*(arr[loc1]-arr[loc2]);
			cache[loc1]=tempzeta;
			tempzeta*=zeta;
			++loc2;
			++loc1;
		}
	}
	
	for(size_t deg=2;deg<degree;++deg){
		size_t looplength=(size_t)1<<(degree-deg);
		size_t loopnum=(size_t)1<<(deg-1);
		size_t loc1=0;
		size_t loc2=looplength;
		size_t cache_dif=loopnum;
		Mod<P>* dist=res[deg];
		Mod<P>* olddist=res[deg-1];
		for(size_t i=0;i<loopnum;++i){
			size_t cache_loc=0;
			for(size_t j=0;j<looplength;++j){
				dist[loc1]=olddist[loc1]+olddist[loc2];
				dist[loc2]=cache[cache_loc]*(olddist[loc1]-olddist[loc2]);
				cache_loc+=cache_dif;
				++loc1;
				++loc2;
			}
			loc1=loc2;
			loc2=loc1+looplength;
			
		}
	}
	
	{
		size_t deg=degree;
		size_t looplength=(size_t)1<<(degree-deg);
		size_t loopnum=(size_t)1<<(deg-1);
		size_t loc1=0;
		size_t loc2=looplength;
		Mod<P>* dist=res[deg];
		Mod<P>* olddist=res[deg-1];
		for(size_t i=0;i<loopnum;++i){
			for(size_t j=0;j<looplength;++j){
				dist[loc1]=olddist[loc1]+olddist[loc2];
				dist[loc2]=(olddist[loc1]-olddist[loc2]);
				++loc1;
				++loc2;
			}
			loc1=loc2;
			loc2=loc1+looplength;
		}
	}
	Mod<P>* result=res[degree];
	for(size_t i=1;i<degree;++i){
		delete[] res[i];
	}
	delete[] res;
	return LightArray<Mod<P>>(result,length);
}


template <size_t P>
inline LightArray<Mod<P>> fouriertransform_time(Array<Mod<P>>& arr,size_t degree,Mod<P> zeta){
	size_t length=1<<degree;
	
	Mod<P>** res=new (std::nothrow) Mod<P>*[degree+1];
	if(res==nullptr) std::exit(EXIT_FAILURE);
	for(size_t i=1;i<=degree;++i){
		res[i]=new (std::nothrow) Mod<P>[length];
		if(res[i]==nullptr) std::exit(EXIT_FAILURE);
	}
	
	Mod<P> tempzeta;
	tempzeta=1;
	LightArray<Mod<P>> cache;
	cache.Allocate(length>>1);
	{
		size_t looplength=length>>1;
		for(size_t i=0;i<looplength;++i){
			cache[i]=tempzeta;
			tempzeta*=zeta;
		}
	}
	{
		size_t deg=1;
		size_t looplength=(size_t)1<<(deg-1);
		size_t loopnum=(size_t)1<<(degree-deg-1);
		size_t loc1=0;
		size_t loc2=looplength;
		size_t cache_dif=(size_t)1<<(degree-deg-1);
		Mod<P>* dist=res[deg];
		Array<Mod<P>>& olddist=arr;
		for(size_t i=0;i<loopnum;++i){
			for(size_t j=0;j<looplength;++j){
				dist[loc1]=olddist[loc1]+olddist[loc2];
				dist[loc2]=(olddist[loc1]-olddist[loc2]);
				++loc1;
				++loc2;
			}
			size_t cache_loc_1=0;
			size_t cache_loc_2=length>>2;
			
			
			loc1=loc2;
			loc2=loc1+looplength;
			for(size_t j=0;j<looplength;++j){
				dist[loc1]=cache[cache_loc_1]*(olddist[loc1]+olddist[loc2]);
				dist[loc2]=cache[cache_loc_2]*(olddist[loc1]-olddist[loc2]);
				++loc1;
				++loc2;
				cache_loc_1+=cache_dif;
				cache_loc_2+=cache_dif;
			}
			
			loc1=loc2;
			loc2=loc1+looplength;
		}
	}
	
	for(size_t deg=2;deg<degree;++deg){
		size_t looplength=(size_t)1<<(deg-1);
		size_t loopnum=(size_t)1<<(degree-deg-1);
		size_t loc1=0;
		size_t loc2=looplength;
		size_t cache_dif=(size_t)1<<(degree-deg-1);
		Mod<P>* dist=res[deg];
		Mod<P>* olddist=res[deg-1];
		for(size_t i=0;i<loopnum;++i){
			for(size_t j=0;j<looplength;++j){
				dist[loc1]=olddist[loc1]+olddist[loc2];
				dist[loc2]=(olddist[loc1]-olddist[loc2]);
				++loc1;
				++loc2;
			}
			size_t cache_loc_1=0;
			size_t cache_loc_2=length>>2;
			
			
			loc1=loc2;
			loc2=loc1+looplength;
			for(size_t j=0;j<looplength;++j){
				dist[loc1]=cache[cache_loc_1]*(olddist[loc1]+olddist[loc2]);
				dist[loc2]=cache[cache_loc_2]*(olddist[loc1]-olddist[loc2]);
				++loc1;
				++loc2;
				cache_loc_1+=cache_dif;
				cache_loc_2+=cache_dif;
			}
			
			loc1=loc2;
			loc2=loc1+looplength;
		}
	}
	{
		size_t deg=degree;
		size_t loc1=0;
		size_t loc2=length>>1;
		Mod<P>* dist=res[deg];
		Mod<P>* olddist=res[deg-1];
		size_t looplength=length>>1;
		for(size_t j=0;j<looplength;++j){
			dist[loc1]=olddist[loc1]+olddist[loc2];
			dist[loc2]=(olddist[loc1]-olddist[loc2]);
			++loc1;
			++loc2;
		}
		
	}
	
	
	
	
	Mod<P>* result=res[degree];
	for(size_t i=1;i<degree;++i){
		delete[] res[i];
	}
	delete[] res;
	return LightArray<Mod<P>>(result,length);
}



template <size_t P>
inline LightArray<Mod<P>> fouriertransform(Array<Mod<P>>& arr,size_t degree,Mod<P> zeta){
	LightArray<LightArray<Mod<P>>> res;
	size_t length=(size_t)1<<degree;
	res.Allocate(degree+1);
	for(size_t i=0;i<=degree;++i) res[i].Allocate(length);
	for(size_t i=0;i<=degree;++i){
		Mod<P> epsilon=Power(zeta,(length>>i));
		if(i==0){
			for(size_t j=0;j<length;++j){
				res[i][j]=arr[j];
			}
		}else{
			LightArray<Mod<P>> temp((size_t)1<<(i));
			Mod<P> counter=1;
			for(size_t j=0;j<(size_t)1<<(i);++j){
				temp[j]=counter;
				counter*=epsilon;
			}
			
			for(size_t j=0;j<(length>>i);++j){//j:start point, to next is length>>i
				size_t loc=j*((size_t)1<<(i));
				size_t loc1=j*((size_t)1<<(i-1));
				size_t loc2=(j+(length>>i))*((size_t)1<<(i-1));
				for(size_t k=0;k<((size_t)1<<(i-1));++k){//k:\epsilon^(k),
					res[i][loc]=res[i-1][loc1]+temp[k]*res[i-1][loc2];
					++loc;
					++loc1;
					++loc2;
					
				}
				loc1=j*((size_t)1<<(i-1));
				loc2=(j+(length>>i))*((size_t)1<<(i-1));
				for(size_t k=((size_t)1<<(i-1));k<((size_t)1<<(i));++k){//k:\epsilon^(k),
					res[i][loc]=res[i-1][loc1]+temp[k]*res[i-1][loc2];
					++loc;
					++loc1;
					++loc2;
					
				}
			}
		}
		
	}
	/*
			for(size_t j=0;j<(length>>i);++j){//j:start point, to next is length>>i
				for(size_t k=0;k<((size_t)1<<(i-1));++k){//k:\epsilon^(k),
					Mod<P> temp=Power(epsilon,k);
					res[i][j*((size_t)1<<(i))+k]=res[i-1][j*((size_t)1<<(i-1))+k]+temp*res[i-1][(j+(length>>i))*((size_t)1<<(i-1))+k];
				}
			
				for(size_t k=0;k<((size_t)1<<(i-1));++k){//k:\epsilon^(k),
					Mod<P> temp=Power(epsilon,k+((size_t)1<<(i-1)));
					res[i][j*((size_t)1<<(i))+(k+((size_t)1<<(i-1)))]=res[i-1][j*((size_t)1<<(i-1))+k]+temp*res[i-1][(j+(length>>i))*((size_t)1<<(i-1))+k];
				}
			}
	*/
	
	LightArray<Mod<P>> resarr=std::move(res[degree]);
	
	return resarr;
}

template <size_t P>
inline Array<Mod<P>> oldfouriertransform(Array<Mod<P>>& arr,size_t totaldegree,size_t degree,size_t start,Mod<P> zeta){
	Array<Mod<P>> res;
	res.Allocate((size_t)1<<(totaldegree-degree));
	if(totaldegree==degree){
		res.Push(arr[start]);
		return res;
	}
	Array<Mod<P>> arr1=oldfouriertransform(arr,totaldegree,degree+1,start,zeta*zeta);
	Array<Mod<P>> arr2=oldfouriertransform(arr,totaldegree,degree+1,start+((size_t)1<<degree),zeta*zeta);
	Mod<P> counter=1;
	for(size_t i=0;i<(size_t)1<<(totaldegree-degree-1);++i){
		res.Push(arr1[i]+counter*arr2[i]);
		counter=counter*zeta;
	}
	for(size_t i=0;i<(size_t)1<<(totaldegree-degree-1);++i){
		res.Push(arr1[i]+counter*arr2[i]);
		counter=counter*zeta;
	}
	
	return res;
}


//base is (1,1,1,),(1,\zeta,\zeta^2,....)
template <size_t P>
inline LightArray<Mod<P>> FourierTransform(Array<Mod<P>>& arr,size_t degree,Mod<P> zeta){
	LightArray<Mod<P>> res=fouriertransform(arr,degree,Mod<P>().Set(1)/zeta);
	
	Mod<P> div;
	div.Set((size_t)1<<degree);
	div=div.Inv();
	for(size_t i=0;i<res.Length;++i){
		res[i]*=div;
		//res[i]=res[i]/Mod<P>((size_t)1<<degree);
	}
	return res;
}


//base is (1,1,1,),(1,\zeta,\zeta^2,....)
template <size_t P>
inline LightArray<Mod<P>> InverseFourierTransform(Array<Mod<P>>& arr,size_t degree,Mod<P> zeta){
	LightArray<Mod<P>> res=fouriertransform(arr,degree,zeta);
	return res;
}

template <ull P>
inline Mod<P> Factorial(ull n){
	static Array<Mod<P>> arr;
	if(arr.Length<=n){
		for(ull i=arr.Length;i<=n;++i){
			if(i==0) arr[i].Set(1);
			else{
				arr[i]=arr[i-1]*Mod<P>().Set(i);
			}
		}
	}
	return arr[n];
}

template <std::integral T>
inline T Comb(T n,T m){
	assert(n>0 && m>=0);
	T counter=One<T>;
	for(T i=1;i<=m;++i){
		counter*=n-i+One<T>;
		counter/=i;
	}
	return counter;

}

template <ull P>
inline Mod<P> Comb(ull n,ull m){
	static_assert(IsPrime(P));
	return Factorial<P>(n)/Factorial<P>(m)/Factorial<P>(n-m);
}


using NMod=Mod<NiceP>;

template <ull P>
inline Mod<P> mod(ull n){
	return Mod<P>().Set(n);
}

inline Mod<NiceP> nmod(ull n){return mod<NiceP>(n);}

