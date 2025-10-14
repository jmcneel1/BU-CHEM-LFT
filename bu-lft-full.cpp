#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <sstream>
#include <complex>

struct CSFType
{
    int count;
    short spin;
    std::string dets;
    std::string coeffs;
};

std::string HModel (short s, short ms1, short ms2 )
{
    std::string result = "";
    if ( s == 2 )
    {
        if ( ms1 == 2 )
        {
            if ( ms2 == 2 ) result = "0.5(Dxx+Dyy)+Dzz";
            else if ( ms2 == 0) result = "0.707(Dxz+iDyz)";
            else result = "0.5(Dxx-Dyy+2iDxy)";
        }
        else if ( ms1 == 0 )
        {
            if ( ms2 == 2 ) result = "0.707(Dxz-iDyz)";
            else if ( ms2 == 0 ) result = "Dxx+Dyy";
            else result = "-0.707(Dxz+iDyz)";
        }
        else
        {
            if ( ms2 == 2 ) result = "0.5(Dxx-Dyy-iDxy)";
            else if ( ms2 == 0 ) result = "-0.707(Dxz-iDyz)";
            else result = "-0.5(Dxx+Dyy)+Dzz";
        }
    }
    else if ( s == 3 )
    {
        if ( ms1 == 3 )
        {
            if ( ms2 == 3 ) result = "0.75(Dxx+Dyy+3Dzz)";
            else if ( ms2 == 1) result = "1.732(Dxz+iDyz)";
            else if ( ms2 == -1 ) result = "0.866(Dxx-Dyy+2iDxy)";
            else result = "0";
        }
        else if ( ms1 == 1 )
        {
            if ( ms2 == 3 ) result = "1.732(Dxz-iDyz)";
            else if ( ms2 == 1 ) result = "0.25(7Dxx+7Dyy+Dzz)";
            else if ( ms2 == -1 ) result = "0";
            else result = "0.866(Dxx-Dyy+2iDxy)";
        }
        else if ( ms1 == -1 )
        {
            if ( ms2 == 3 ) result = "0.866(Dxx-Dyy-2iDxy)";
            else if ( ms2 == 1 ) result = "0";
            else if ( ms2 == -1 ) result = "0.25(7Dxx+7Dyy+Dzz)";
            else result = "-1.732(Dxz+iDyz)";
        }
        else
        {
            if ( ms2 == 3 ) result = "0";
            else if ( ms2 == 1 ) result = "0.866(Dxx-Dyy-2iDxy)";
            else if ( ms2 == -1 ) result = "-1.732(Dxz-iDyz)";
            else result = "0.75(Dxx+Dyy+3Dzz)";
        }
    }
    else if ( s == 4 )
    {
        if ( ms1 == 4 )
        {
            if ( ms2 == 4 ) result = "Dxx+Dyy+4Dzz";
            else if ( ms2 == 2) result = "3(Dxz+iDyz)";
            else if ( ms2 == 0 ) result = "1.225(Dxx-Dyy+2iDxy)";
            else if ( ms2 == -2 ) result = "0";
            else result = "0";
        }
        else if ( ms1 == 2 )
        {
            if ( ms2 == 4 ) result = "3(Dxz-iDyz)";
            else if ( ms2 == 2) result = "0.5(5Dxx+5Dyy+2Dzz)";
            else if ( ms2 == 0 ) result = "1.225(Dxz+iDyz)";
            else if ( ms2 == -2 ) result = "1.5(Dxx-Dyy+2iDxy)";
            else result = "0";
        }
        else if ( ms1 == 0 )
        {
            if ( ms2 == 4 ) result = "1.225(Dxx-Dyy-2iDxy)";
            else if ( ms2 == 2) result = "1.225(Dxz-iDyz)";
            else if ( ms2 == 0 ) result = "3(Dxx+Dyy)";
            else if ( ms2 == -2 ) result = "-1.225(Dxz-iDyz)";
            else result = "1.225(Dxx-Dyy+2iDxy)";
        }
        else if ( ms1 == -2 )
        {
            if ( ms2 == 4 ) result = "0";
            else if ( ms2 == 2) result = "1.5(Dxx-Dyy-2iDxy)";
            else if ( ms2 == 0 ) result = "-1.225(Dxz-Dyz)";
            else if ( ms2 == -2 ) result = "0.5(5Dxx+5Dyy+2Dzz)";
            else result = "-3(Dxz+iDyz)";
        }
        else
        {
            if ( ms2 == 4 ) result = "0";
            else if ( ms2 == 2) result = "0";
            else if ( ms2 == 0 ) result = "1.225(Dxx-Dyy-2iDxy)";
            else if ( ms2 == -2 ) result = "-3(Dxz-iDyz)";
            else result = "Dxx+Dyy+4Dzz";
        }
    }
    else
    {
        if ( ms1 == 5 )
        {
            if ( ms2 == 5 ) result = "1.25(Dxx+Dyy+5Dzz)";
            else if ( ms2 == 3) result = "4.472(Dxz+iDyz)";
            else if ( ms2 == 1 ) result = "1.581(Dxx-Dyy+2iDxy)";
            else if ( ms2 == -1 ) result = "0";
            else if ( ms2 == -3 ) result = "0";
            else result = "0";
        }
        else if ( ms1 == 3 )
        {
            if ( ms2 == 5 ) result = "4.472(Dxz-iDyz)";
            else if ( ms2 == 3) result = "0.25(13Dxx+13Dyy+9Dzz)";
            else if ( ms2 == 1 ) result = "2.828(Dxz+iDyz)";
            else if ( ms2 == -1 ) result = "2.121(Dxx-Dyy+2iDxy)";
            else if ( ms2 == -3 ) result = "0";
            else result = "0";
        }
        else if ( ms1 == 1 )
        {
            if ( ms2 == 5 ) result = "1.581(Dxx-Dyy-2iDxy)";
            else if ( ms2 == 3) result = "2.828(Dxz-iDyz)";
            else if ( ms2 == 1 ) result = "0.25(17Dxx+17Dyy+Dzz)";
            else if ( ms2 == -1 ) result = "0";
            else if ( ms2 == -3 ) result = "2.121(Dxx-Dyy+2iDxy)";
            else result = "0";
        }
        else if ( ms1 == -1 )
        {
            if ( ms2 == 5 ) result = "0";
            else if ( ms2 == 3) result = "2.121(Dxx-Dyy-2iDxy)";
            else if ( ms2 == 1 ) result = "0";
            else if ( ms2 == -1 ) result = "0.25(17Dxx+17Dyy+Dzz)";
            else if ( ms2 == -3 ) result = "-2.828(Dxz+iDyz)";
            else result = "1.581(Dxx-Dyy+2iDxy)";
        }
        else if ( ms1 == -3 )
        {
            if ( ms2 == 5 ) result = "0";
            else if ( ms2 == 3) result = "0";
            else if ( ms2 == 1 ) result = "2.121(Dxx-Dyy-2iDxy)";
            else if ( ms2 == -1 ) result = "-2.828(Dxz-iDyz)";
            else if ( ms2 == -3 ) result = "0.25(13Dxx+13Dyy+9Dzz)";
            else result = "-4.472(Dxz+iDyz)";
        }
        else
        {
            if ( ms2 == 5 ) result = "0";
            else if ( ms2 == 3) result = "0";
            else if ( ms2 == 1 ) result = "0";
            else if ( ms2 == -1 ) result = "1.581(Dxx-Dyy-2iDxy)";
            else if ( ms2 == -3 ) result = "-4.472(Dxz-iDyz)";
            else result = "1.25(Dxx+Dyy+5Dzz)";
        }
    }
    return result;
}

int GetIndex ( const CSFType & csf, const int & id )
{
    int index(0), opos(0), npos(0);
    std::stringstream ss;
    for ( unsigned int i = 0; i < id; i++ )
    {
        npos = csf.dets.find('-',opos+1);
        opos=npos+1;
    }
    npos = csf.dets.find('-',opos+1);
    ss << csf.dets.substr(opos,npos-opos);
    ss >> index;
    return index;
}

double GetCoeff ( const CSFType & csf, const int & id )
{
    int opos(0), npos(0);
    double coeff;
    std::stringstream ss;
    for ( unsigned int i = 0; i < id; i++ )
    {
        npos = csf.coeffs.find('_',opos+1);
        opos=npos+1;
    }
    npos = csf.coeffs.find('_',opos+1);
    ss << csf.coeffs.substr(opos,npos-opos);
    ss >> coeff;
    return coeff;
}

//  t is true if +1/2, false if -1/2
// sigma is true if +1/2, false if -1/2

double Clebsch ( bool t, bool sigma, short st2, short mt2 )
{
    double s, m;
    s = st2/2.0;
    m = mt2/2.0;

    if ( t )
    {
        if ( sigma ) return std::pow((s+m)/(2*s),0.5);
        else return std::pow((s-m)/(2*s),0.5);
    }
    else
    {
        if ( sigma ) return -std::pow((s+1-m)/(2*(s+1)),0.5);
        else return std::pow((s+1+m)/(2*(s+1)),0.5);
    }

}

int Factorial ( const int & num )
{
    if ( num > 0 ) return num * Factorial(num-1);
    else return 1;
}

int Binomial ( int n, int k )
{
    return Factorial(n)/(Factorial(k)*Factorial(n-k));
}

int NumCSF ( short st2, short nel )
{
    double result, k1content, k2content;
    int k1, k2;
    k1content = -nel/2.0 - st2/2.0;
    k2content = nel/2.0 - st2/2.0;
    k1 = 5 + std::floor(k1content);
    k2 = std::floor(k2content);
    result = (st2+1.0)/6.0*Binomial(6,k1)*Binomial(6,k2);
    return std::floor(result);
}

void GenerateDeterminants ( std::vector< std::vector <bool> > & dets,
                            const short & nel
                          )
{

    unsigned int index = 0;
    for ( unsigned int e1 = 0; e1 < 10; e1++ )
    {
        if ( nel > 1 )
        {
            for ( unsigned int e2 = e1+1; e2 < 10; e2++ )
            {
                if ( nel > 2 )
                {
                    for ( unsigned int e3 = e2+1; e3 < 10; e3++ )
                    {
                        if ( nel > 3 )
                        {
                            for ( unsigned int e4 = e3+1; e4 < 10; e4++ )
                            {
                                if ( nel > 4 )
                                {
                                    for ( unsigned int e5 = e4+1; e5 < 10; e5++ )
                                    {
                                        if ( nel > 5 )
                                        {
                                            for ( unsigned int e6 = e5+1; e6 < 10; e6++ )
                                            {
                                                if ( nel > 6 )
                                                {
                                                    for ( unsigned int e7 = e6+1; e7 < 10; e7++ )
                                                    {
                                                        if ( nel > 7 )
                                                        {
                                                            for ( unsigned int e8 = e7+1; e8 < 10; e8++ )
                                                            {
                                                                if ( nel > 8 )
                                                                {
                                                                    if ( nel > 9 )
                                                                    {
                                                                        std::vector<bool> temp(10,true);
                                                                        dets[index] = temp;
                                                                        index++;
                                                                    }
                                                                    else
                                                                    {
                                                                        for ( unsigned int e9 = e8+1; e9 < 10; e9++ )
                                                                        {
                                                                            std::vector<bool> temp(10,false);
                                                                            temp[e1] = temp[e2] = temp[e3] = temp[e4] = temp[e5] = temp[e6] = temp[e7] = temp[e8] = true;
                                                                            dets[index] = temp;
                                                                            index++;
                                                                        }
                                                                    }
                                                                }
                                                                else
                                                                {
                                                                    std::vector<bool> temp(10,false);
                                                                    temp[e1] = temp[e2] = temp[e3] = temp[e4] = temp[e5] = temp[e6] = temp[e7] = temp[e8] = true;
                                                                    dets[index] = temp;
                                                                    index++;
                                                                }
                                                            }
                                                        }
                                                        else
                                                        {
                                                            std::vector<bool> temp(10,false);
                                                            temp[e1] = temp[e2] = temp[e3] = temp[e4] = temp[e5] = temp[e6] = temp[e7] = true;
                                                            dets[index] = temp;
                                                            index++;
                                                        }
                                                    }
                                                }
                                                else
                                                {
                                                    std::vector<bool> temp(10,false);
                                                    temp[e1] = temp[e2] = temp[e3] = temp[e4] = temp[e5] = temp[e6] = true;
                                                    dets[index] = temp;
                                                    index++;
                                                }
                                            }
                                        }
                                        else
                                        {
                                            std::vector<bool> temp(10,false);
                                            temp[e1] = temp[e2] = temp[e3] = temp[e4] = temp[e5] = true;
                                            dets[index] = temp;
                                            index++;
                                        }
                                    }
                                }
                                else
                                {
                                    std::vector<bool> temp(10,false);
                                    temp[e1] = temp[e2] = temp[e3] = temp[e4] = true;
                                    dets[index] = temp;
                                    index++;
                                }
                            }
                        }
                        else
                        {
                            std::vector<bool> temp(10,false);
                            temp[e1] = temp[e2] = temp[e3] = true;
                            dets[index] = temp;
                            index++;
                        }
                    }
                }
                else
                {
                    std::vector<bool> temp(10, false);
                    temp[e1] = temp[e2] = true;
                    dets[index] = temp;
                    index++;
                }
            }
        }
        else
        {
            std::vector<bool> temp(10, false);
            temp[e1] = true;
            dets[index] = temp;
            index++; 
        }
    }

}

void SetDetMs (
                const std::vector<std::vector<bool>> & dets,
                std::vector<short> & ms,
                const int & n_dets
             )
{
    for ( unsigned int i = 0; i < n_dets; i++ )
    {
        short tms(0);
        for ( unsigned int j = 0; j < 10; j++ )
        {
            if ( dets[i][j] ) tms+=std::pow(-1,j);
        }
        ms[i] = tms;
    }
}

void SetDetConfig (
                    const std::vector<std::vector<bool>> & dets,
                    std::vector<std::vector<short>> & configs,
                    const int & n_dets
                  )
{
    for ( unsigned int i = 0; i < n_dets; i++ )
    {
        for ( unsigned int j = 0; j < 5; j++ )
        {
            if ( dets[i][j*2] ) configs[i][j]++;
            if ( dets[i][j*2+1] ) configs[i][j]++;
        }
    }
}

void GenerateCoeffs ( 
                      CSFType & tcsf,
                      const std::vector<std::vector<bool>> & dets,
                      const std::vector<short> & config,
                      const short & spin,
                      const std::vector<bool> & cpl
                    )
{
    if ( tcsf.count > 1 )
    {
        tcsf.coeffs = "";
        double total;
        for ( unsigned int i = 0; i < tcsf.count; i++ )
        {
            int tindex;
            tindex = GetIndex(tcsf,i);
            bool t; bool sigma; short st2(0); short mt2(0); short cpl_index(0);
            total = 1.0;
            for ( unsigned int j = 0; j < 5; j++ )
            {
                if ( config[j] == 1 )
                {
                    if ( cpl[cpl_index] ) st2++;
                    else st2--;
                    if ( dets[tindex][2*j] )
                    {
                        mt2++;
                        total*=Clebsch(cpl[cpl_index],true,st2,mt2);
                        cpl_index++;
                    }
                    else
                    {
                        mt2--;
                        total*=Clebsch(cpl[cpl_index],false,st2,mt2);
                        cpl_index++;
                    }
                }
            }
            std::stringstream ss;
            std::string tot_str;
            ss << std::fixed << std::setprecision(4) << total;
            ss >> tot_str;
            tcsf.coeffs += tot_str;
            if ( i <  (tcsf.count -1) ) tcsf.coeffs+="_";
        }
    }
}

std::vector<std::vector<bool>> GenerateCouplingVecs( short spin, short nel)
{
    short unp;
    std::vector<std::vector<bool>> coups;
    if ( nel > 5 ) unp = 10-nel;
    else unp = nel;
    for ( int k = unp; k >= spin; k-=2 )
    {
        int total = 1 << k;
        std::vector<bool> temp(k);
        for ( unsigned int i = 0; i < total; i++ )
        {
            short temp_spin(0);
            for ( int j = k - 1; j >= 0; j-- )
            {
                temp[k-j-1] = ((i >> j) & 1) != 0;
                temp_spin += (temp[k-j-1]) ? 1 : -1;
                if ( temp_spin < 0 ) break;
            }
            if ( temp_spin == spin ) coups.push_back(temp);
        }
    }
    return coups;
}

void GenerateCSFS (
                    std::vector<CSFType> & csfs, 
                    const std::vector<short> & spins,
                    const std::vector<std::vector<bool>> & dets,
                    const std::vector<std::vector<short>> & configs,
                    const std::vector<short> & det_ms,
                    const int & n_dets,
                    const short & nel
                  )
{
    int index = 0;
    for ( auto spin : spins )
    {
        std::vector<std::vector<bool>> coupling_vecs = GenerateCouplingVecs(spin,nel);
        std::cout << "There are " << coupling_vecs.size() << " coupling vectors for S=" << double(spin/2.0) << "\n";
        std::cout << "   ";
        for ( unsigned int i = 0; i < coupling_vecs.size(); i++ )
        {
            std::cout << " |";
            for ( unsigned int j = 0; j < coupling_vecs[i].size(); j++ )
            {
                if ( coupling_vecs[i][j] ) std::cout << "+";
                else std::cout << "-";
            }
            std::cout << ">";
        }
        std::cout << "\n\n";
        for ( auto cpl : coupling_vecs )
        {
            for ( short ms=spin; ms >= -spin; ms-=2 )
            {
                std::vector<bool> found(n_dets,false);
                std::vector<short> tconfig(5,0);
                
                for ( unsigned int i = 0; i < n_dets; i++ )
                {
                    if ( ( ms == det_ms[i] ) && ( !found[i] ) )
                    {
                        short unp(0);
                        for ( unsigned int j = 0; j < 5; j++ )
                        {
                            if ( configs[i][j] == 1 ) unp++;
                        }
                        if ( cpl.size() == unp )
                        {
                            CSFType tcsf;
                            tcsf.count = 1;
                            tcsf.dets=std::to_string(i);
                            tcsf.coeffs="1.0000";
                            tcsf.spin = spin;
                            found[i] = true;
                            for ( unsigned int j = i+1; j < n_dets; j++ )
                            {
                                if ( ms == det_ms[j] && ! found[j] )
                                {
                                    bool same = true;
                                    for ( unsigned int k = 0; k < 5; k++ )
                                    {
                                        if ( configs[i][k] != configs[j][k] )
                                        {
                                            same = false;
                                            break;
                                        }
                                    }
                                    if ( same )
                                    {
                                        found[j] = true;
                                        tcsf.count++;
                                        tcsf.dets+="-"+std::to_string(j);
                                        tcsf.coeffs+="_1.0000";
                                    }
                                }
                            }
                            GenerateCoeffs(tcsf,dets,configs[i],spin,cpl);
                            csfs[index] = tcsf;
                            index++;
                        }
                    }
                }
            }
        }
    }
}

std::complex<double> LX ( const CSFType & lhs, const CSFType & rhs, const std::vector<std::vector<bool>> & dets )
{
    std::stringstream ss;
    double rtotal(0.0), itotal(0.0);
    for ( unsigned int i = 0; i < lhs.count; i++ )
    {
        int lindex(0);
        double lcoeff(0.0);
        lindex = GetIndex(lhs,i);
        lcoeff = GetCoeff(lhs,i);
        for ( unsigned int j = 0; j < rhs.count; j++)
        {
            int rindex(0);
            double rcoeff(0.0);
            rindex = GetIndex(rhs,j);
            rcoeff = GetCoeff(rhs,j);
            for ( unsigned int k = 0; k < 10; k++ )
            {
                std::vector<bool> temp = dets[rindex];
                if ( k == 0 ) 
                {
                    if ( temp[0] )
                    {
                        if ( !temp[4] )
                        {
                            temp[4] = true; temp[0] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal - lcoeff * rcoeff;
                            }
                        }
                    }
                }
                else if ( k == 1 )
                {
                    if ( temp[1] )
                    {
                        if ( !temp[5] )
                        {
                            temp[5] = true; temp[1] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal - lcoeff * rcoeff;
                            }
                        }
                    }
                }
                else if ( k == 2 )
                {
                    if ( temp[2] )
                    {
                        if ( !temp[6] )
                        {
                            temp[6] = true; temp[2] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal + lcoeff * rcoeff * std::pow(3.0,0.5);
                            }
                            temp[6] = false; temp[2] = true;
                        }
                        if ( !temp[8] )
                        {
                            temp[8] = true; temp[2] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal - lcoeff * rcoeff;
                            }
                        }
                    }
                }
                else if ( k == 3 )
                {
                    if ( temp[3] )
                    {
                        if ( !temp[7] )
                        {
                            temp[7] = true; temp[3] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal + lcoeff * rcoeff * std::pow(3.0,0.5);
                            }
                            temp[7] = false; temp[3] = true;
                        }
                        if ( !temp[9] )
                        {
                            temp[9] = true; temp[3] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal - lcoeff * rcoeff;
                            }
                        }
                    }
                }
                else if ( k == 4 )
                {
                    if ( temp[4] )
                    {
                        if ( !temp[0] )
                        {
                            temp[0] = true; temp[4] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal + lcoeff * rcoeff;
                            }
                        }
                    }
                }
                else if ( k == 5 )
                {
                    if ( temp[5] )
                    {
                        if ( !temp[1] )
                        {
                            temp[1] = true; temp[5] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal + lcoeff * rcoeff;
                            }
                        }
                    }
                }
                else if ( k == 6 )
                {
                    if ( temp[6] )
                    {
                        if ( !temp[2] )
                        {
                            temp[2] = true; temp[6] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal - lcoeff * rcoeff * std::pow(3.0,0.5);
                            }
                        }
                    }
                }
                else if ( k == 7 )
                {
                    if ( temp[7] )
                    {
                        if ( !temp[3] )
                        {
                            temp[3] = true; temp[7] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal - lcoeff * rcoeff * std::pow(3.0,0.5);
                            }
                        }
                    }
                }
                else if ( k == 8 )
                {
                    if ( temp[8] )
                    {
                        if ( !temp[2] )
                        {
                            temp[2] = true; temp[8] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal + lcoeff * rcoeff;
                            }
                        }
                    }
                }
                else
                {
                    if ( temp[9] )
                    {
                        if ( !temp[3] )
                        {
                            temp[3] = true; temp[9] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal + lcoeff * rcoeff;
                            }
                        }
                    }
                }
            }
        }
    }
    std::complex<double> total(rtotal,itotal);
    return total;
}

std::complex<double> LXSX ( const CSFType & lhs, const CSFType & rhs, const std::vector<std::vector<bool>> & dets )
{
    std::stringstream ss;
    double rtotal(0.0), itotal(0.0);
    for ( unsigned int i = 0; i < lhs.count; i++ )
    {
        int lindex(0);
        double lcoeff(0.0);
        lindex = GetIndex(lhs,i);
        lcoeff = GetCoeff(lhs,i);
        for ( unsigned int j = 0; j < rhs.count; j++)
        {
            int rindex(0);
            double rcoeff(0.0);
            rindex = GetIndex(rhs,j);
            rcoeff = GetCoeff(rhs,j);
            for ( unsigned int k = 0; k < 10; k++ )
            {
                std::vector<bool> temp = dets[rindex];
                if ( k == 0 ) 
                {
                    if ( temp[0] )
                    {
                        if ( !temp[5] )
                        {
                            temp[5] = true; temp[0] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal - 0.5*(lcoeff * rcoeff);
                            }
                        }
                    }
                }
                else if ( k == 1 )
                {
                    if ( temp[1] )
                    {
                        if ( !temp[4] )
                        {
                            temp[4] = true; temp[1] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal - 0.5*(lcoeff * rcoeff);
                            }
                        }
                    }
                }
                else if ( k == 2 )
                {
                    if ( temp[2] )
                    {
                        if ( !temp[7] )
                        {
                            temp[7] = true; temp[2] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal + 0.5*(lcoeff * rcoeff * std::pow(3.0,0.5));
                            }
                            temp[7] = false; temp[2] = true;
                        }
                        if ( !temp[9] )
                        {
                            temp[9] = true; temp[2] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal - 0.5*(lcoeff * rcoeff);
                            }
                        }
                    }
                }
                else if ( k == 3 )
                {
                    if ( temp[3] )
                    {
                        if ( !temp[6] )
                        {
                            temp[6] = true; temp[3] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal + 0.5*(lcoeff * rcoeff * std::pow(3.0,0.5));
                            }
                            temp[6] = false; temp[3] = true;
                        }
                        if ( !temp[8] )
                        {
                            temp[8] = true; temp[3] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal - 0.5*(lcoeff * rcoeff);
                            }
                        }
                    }
                }
                else if ( k == 4 )
                {
                    if ( temp[4] )
                    {
                        if ( !temp[1] )
                        {
                            temp[1] = true; temp[4] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal + 0.5*(lcoeff * rcoeff);
                            }
                        }
                    }
                }
                else if ( k == 5 )
                {
                    if ( temp[5] )
                    {
                        if ( !temp[0] )
                        {
                            temp[0] = true; temp[5] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal + 0.5*(lcoeff * rcoeff);
                            }
                        }
                    }
                }
                else if ( k == 6 )
                {
                    if ( temp[6] )
                    {
                        if ( !temp[3] )
                        {
                            temp[3] = true; temp[6] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal - 0.5*(lcoeff * rcoeff * std::pow(3.0,0.5));
                            }
                        }
                    }
                }
                else if ( k == 7 )
                {
                    if ( temp[7] )
                    {
                        if ( !temp[2] )
                        {
                            temp[2] = true; temp[7] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal - 0.5*(lcoeff * rcoeff * std::pow(3.0,0.5));
                            }
                        }
                    }
                }
                else if ( k == 8 )
                {
                    if ( temp[8] )
                    {
                        if ( !temp[3] )
                        {
                            temp[3] = true; temp[8] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal + 0.5*(lcoeff * rcoeff);
                            }
                        }
                    }
                }
                else
                {
                    if ( temp[9] )
                    {
                        if ( !temp[2] )
                        {
                            temp[2] = true; temp[9] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal + 0.5*(lcoeff * rcoeff);
                            }
                        }
                    }
                }
            }
        }
    }
    std::complex<double> total(rtotal,itotal);
    return total;
}

std::complex<double> LY ( const CSFType & lhs, const CSFType & rhs, const std::vector<std::vector<bool>> & dets )
{
    std::stringstream ss;
    double rtotal(0.0), itotal(0.0);
    for ( unsigned int i = 0; i < lhs.count; i++ )
    {
        int lindex(0);
        double lcoeff(0.0);
        lindex = GetIndex(lhs,i);
        lcoeff = GetCoeff(lhs,i);
        for ( unsigned int j = 0; j < rhs.count; j++)
        {
            int rindex(0);
            double rcoeff(0.0);
            rindex = GetIndex(rhs,j);
            rcoeff = GetCoeff(rhs,j);
            for ( unsigned int k = 0; k < 10; k++ )
            {
                std::vector<bool> temp = dets[rindex];
                if ( k == 0 ) 
                {
                    if ( temp[0] )
                    {
                        if ( !temp[6] )
                        {
                            temp[6] = true; temp[0] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal + lcoeff * rcoeff * std::pow(3.0,0.5);
                            }
                            temp[0] = true; temp[6] = false;
                        }
                        if ( !temp[8] )
                        {
                            temp[8] = true; temp[0] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal + lcoeff * rcoeff;
                            }
                        }
                    }
                }
                else if ( k == 1 )
                {
                    if ( temp[1] )
                    {
                        if ( !temp[7] )
                        {
                            temp[7] = true; temp[1] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal + lcoeff * rcoeff * std::pow(3.0,0.5);
                            }
                            temp[1] = true; temp[7] = false;
                        }
                        if ( !temp[9] )
                        {
                            temp[9] = true; temp[1] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal + lcoeff * rcoeff;
                            }
                        }
                    }
                }
                else if ( k == 2 )
                {
                    if ( temp[2] )
                    {
                        if ( !temp[4] )
                        {
                            temp[4] = true; temp[2] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal - lcoeff * rcoeff;
                            }
                        }
                    }
                }
                else if ( k == 3 )
                {
                    if ( temp[3] )
                    {
                        if ( !temp[5] )
                        {
                            temp[5] = true; temp[3] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal - lcoeff * rcoeff;
                            }
                        }
                    }
                }
                else if ( k == 4 )
                {
                    if ( temp[4] )
                    {
                        if ( !temp[2] )
                        {
                            temp[2] = true; temp[4] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal + lcoeff * rcoeff;
                            }
                        }
                    }
                }
                else if ( k == 5 )
                {
                    if ( temp[5] )
                    {
                        if ( !temp[3] )
                        {
                            temp[3] = true; temp[5] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal + lcoeff * rcoeff;
                            }
                        }
                    }
                }
                else if ( k == 6 )
                {
                    if ( temp[6] )
                    {
                        if ( !temp[0] )
                        {
                            temp[0] = true; temp[6] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal - lcoeff * rcoeff * std::pow(3.0,0.5);
                            }
                        }
                    }
                }
                else if ( k == 7 )
                {
                    if ( temp[7] )
                    {
                        if ( !temp[1] )
                        {
                            temp[1] = true; temp[7] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal - lcoeff * rcoeff * std::pow(3.0,0.5);
                            }
                        }
                    }
                }
                else if ( k == 8 )
                {
                    if ( temp[8] )
                    {
                        if ( !temp[0] )
                        {
                            temp[0] = true; temp[8] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal - lcoeff * rcoeff;
                            }
                        }
                    }
                }
                else
                {
                    if ( temp[9] )
                    {
                        if ( !temp[1] )
                        {
                            temp[1] = true; temp[9] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal - lcoeff * rcoeff;
                            }
                        }
                    }
                }
            }
        }
    }
    std::complex<double> total(rtotal,itotal);
    return total;
}

std::complex<double> LYSY ( const CSFType & lhs, const CSFType & rhs, const std::vector<std::vector<bool>> & dets )
{
    std::stringstream ss;
    double rtotal(0.0), itotal(0.0);
    for ( unsigned int i = 0; i < lhs.count; i++ )
    {
        int lindex(0);
        double lcoeff(0.0);
        lindex = GetIndex(lhs,i);
        lcoeff = GetCoeff(lhs,i);
        for ( unsigned int j = 0; j < rhs.count; j++)
        {
            int rindex(0);
            double rcoeff(0.0);
            rindex = GetIndex(rhs,j);
            rcoeff = GetCoeff(rhs,j);
            for ( unsigned int k = 0; k < 10; k++ )
            {
                std::vector<bool> temp = dets[rindex];
                if ( k == 0 ) 
                {
                    if ( temp[0] )
                    {
                        if ( !temp[7] )
                        {
                            temp[7] = true; temp[0] = false;
                            if ( dets[lindex] == temp )
                            {
                                rtotal = rtotal - 0.5*(lcoeff * rcoeff * std::pow(3.0,0.5));
                            }
                            temp[0] = true; temp[7] = false;
                        }
                        if ( !temp[9] )
                        {
                            temp[9] = true; temp[0] = false;
                            if ( dets[lindex] == temp )
                            {
                                rtotal = rtotal - 0.5*(lcoeff * rcoeff);
                            }
                        }
                    }
                }
                else if ( k == 1 )
                {
                    if ( temp[1] )
                    {
                        if ( !temp[6] )
                        {
                            temp[6] = true; temp[1] = false;
                            if ( dets[lindex] == temp )
                            {
                                rtotal = rtotal + 0.5*(lcoeff * rcoeff * std::pow(3.0,0.5));
                            }
                            temp[1] = true; temp[6] = false;
                        }
                        if ( !temp[8] )
                        {
                            temp[8] = true; temp[1] = false;
                            if ( dets[lindex] == temp )
                            {
                                rtotal = rtotal + 0.5*(lcoeff * rcoeff);
                            }
                        }
                    }
                }
                else if ( k == 2 )
                {
                    if ( temp[2] )
                    {
                        if ( !temp[5] )
                        {
                            temp[5] = true; temp[2] = false;
                            if ( dets[lindex] == temp )
                            {
                                rtotal = rtotal + 0.5*(lcoeff * rcoeff);
                            }
                        }
                    }
                }
                else if ( k == 3 )
                {
                    if ( temp[3] )
                    {
                        if ( !temp[4] )
                        {
                            temp[4] = true; temp[3] = false;
                            if ( dets[lindex] == temp )
                            {
                                rtotal = rtotal - 0.5*(lcoeff * rcoeff);
                            }
                        }
                    }
                }
                else if ( k == 4 )
                {
                    if ( temp[4] )
                    {
                        if ( !temp[3] )
                        {
                            temp[3] = true; temp[4] = false;
                            if ( dets[lindex] == temp )
                            {
                                rtotal = rtotal - 0.5*(lcoeff * rcoeff);
                            }
                        }
                    }
                }
                else if ( k == 5 )
                {
                    if ( temp[5] )
                    {
                        if ( !temp[2] )
                        {
                            temp[2] = true; temp[5] = false;
                            if ( dets[lindex] == temp )
                            {
                                rtotal = rtotal + 0.5*(lcoeff * rcoeff);
                            }
                        }
                    }
                }
                else if ( k == 6 )
                {
                    if ( temp[6] )
                    {
                        if ( !temp[1] )
                        {
                            temp[1] = true; temp[6] = false;
                            if ( dets[lindex] == temp )
                            {
                                rtotal = rtotal + 0.5*(lcoeff * rcoeff * std::pow(3.0,0.5));
                            }
                        }
                    }
                }
                else if ( k == 7 )
                {
                    if ( temp[7] )
                    {
                        if ( !temp[0] )
                        {
                            temp[0] = true; temp[7] = false;
                            if ( dets[lindex] == temp )
                            {
                                rtotal = rtotal - 0.5*(lcoeff * rcoeff * std::pow(3.0,0.5));
                            }
                        }
                    }
                }
                else if ( k == 8 )
                {
                    if ( temp[8] )
                    {
                        if ( !temp[1] )
                        {
                            temp[1] = true; temp[8] = false;
                            if ( dets[lindex] == temp )
                            {
                                rtotal = rtotal + 0.5*(lcoeff * rcoeff);
                            }
                        }
                    }
                }
                else
                {
                    if ( temp[9] )
                    {
                        if ( !temp[0] )
                        {
                            temp[0] = true; temp[9] = false;
                            if ( dets[lindex] == temp )
                            {
                                rtotal = rtotal - 0.5*(lcoeff * rcoeff);
                            }
                        }
                    }
                }
            }
        }
    }
    std::complex<double> total(rtotal,itotal);
    return total;
}

std::complex<double> LZ ( const CSFType & lhs, const CSFType & rhs, const std::vector<std::vector<bool>> & dets )
{
    std::stringstream ss;
    double rtotal(0.0), itotal(0.0);
    for ( unsigned int i = 0; i < lhs.count; i++ )
    {
        int lindex(0);
        double lcoeff(0.0);
        lindex = GetIndex(lhs,i);
        lcoeff = GetCoeff(lhs,i);
        for ( unsigned int j = 0; j < rhs.count; j++)
        {
            int rindex(0);
            double rcoeff(0.0);
            rindex = GetIndex(rhs,j);
            rcoeff = GetCoeff(rhs,j);
            for ( unsigned int k = 0; k < 10; k++ )
            {
                std::vector<bool> temp = dets[rindex];
                if ( k == 0 ) 
                {
                    if ( temp[0] )
                    {
                        if ( !temp[2] )
                        {
                            temp[2] = true; temp[0] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal - lcoeff * rcoeff;
                            }
                        }
                    }
                }
                else if ( k == 1 )
                {
                    if ( temp[1] )
                    {
                        if ( !temp[3] )
                        {
                            temp[3] = true; temp[1] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal - lcoeff * rcoeff;
                            }
                        }
                    }
                }
                else if ( k == 2 )
                {
                    if ( temp[2] )
                    {
                        if ( !temp[0] )
                        {
                            temp[0] = true; temp[2] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal + lcoeff * rcoeff;
                            }
                        }
                    }
                }
                else if ( k == 3 )
                {
                    if ( temp[3] )
                    {
                        if ( !temp[1] )
                        {
                            temp[1] = true; temp[3] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal + lcoeff * rcoeff;
                            }
                        }
                    }
                }
                else if ( k == 4 )
                {
                    if ( temp[4] )
                    {
                        if ( !temp[8] )
                        {
                            temp[8] = true; temp[4] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal - 2 * lcoeff * rcoeff;
                            }
                        }
                    }
                }
                else if ( k == 5 )
                {
                    if ( temp[5] )
                    {
                        if ( !temp[9] )
                        {
                            temp[9] = true; temp[5] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal - 2 * lcoeff * rcoeff;
                            }
                        }
                    }
                }
                else if ( k == 8 )
                {
                    if ( temp[8] )
                    {
                        if ( !temp[4] )
                        {
                            temp[4] = true; temp[8] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal + 2 * lcoeff * rcoeff;
                            }
                        }
                    }
                }
                else if ( k == 9 )
                {
                    if ( temp[9] )
                    {
                        if ( !temp[5] )
                        {
                            temp[5] = true; temp[9] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal + 2 * lcoeff * rcoeff;
                            }
                        }
                    }
                }
            }
        }
    }
    std::complex<double> total(rtotal,itotal);
    return total;
}

std::complex<double> LZSZ ( const CSFType & lhs, const CSFType & rhs, const std::vector<std::vector<bool>> & dets )
{
    std::stringstream ss;
    double rtotal(0.0), itotal(0.0);
    for ( unsigned int i = 0; i < lhs.count; i++ )
    {
        int lindex(0);
        double lcoeff(0.0);
        lindex = GetIndex(lhs,i);
        lcoeff = GetCoeff(lhs,i);
        for ( unsigned int j = 0; j < rhs.count; j++)
        {
            int rindex(0);
            double rcoeff(0.0);
            rindex = GetIndex(rhs,j);
            rcoeff = GetCoeff(rhs,j);
            for ( unsigned int k = 0; k < 10; k++ )
            {
                std::vector<bool> temp = dets[rindex];
                if ( k == 0 ) 
                {
                    if ( temp[0] )
                    {
                        if ( !temp[2] )
                        {
                            temp[2] = true; temp[0] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal + 0.5*(lcoeff * rcoeff);
                            }
                        }
                    }
                }
                else if ( k == 1 )
                {
                    if ( temp[1] )
                    {
                        if ( !temp[3] )
                        {
                            temp[3] = true; temp[1] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal - 0.5*(lcoeff * rcoeff);
                            }
                        }
                    }
                }
                else if ( k == 2 )
                {
                    if ( temp[2] )
                    {
                        if ( !temp[0] )
                        {
                            temp[0] = true; temp[2] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal - 0.5*(lcoeff * rcoeff);
                            }
                        }
                    }
                }
                else if ( k == 3 )
                {
                    if ( temp[3] )
                    {
                        if ( !temp[1] )
                        {
                            temp[1] = true; temp[3] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal + 0.5*(lcoeff * rcoeff);
                            }
                        }
                    }
                }
                else if ( k == 4 )
                {
                    if ( temp[4] )
                    {
                        if ( !temp[8] )
                        {
                            temp[8] = true; temp[4] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal - lcoeff * rcoeff;
                            }
                        }
                    }
                }
                else if ( k == 5 )
                {
                    if ( temp[5] )
                    {
                        if ( !temp[9] )
                        {
                            temp[9] = true; temp[5] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal + lcoeff * rcoeff;
                            }
                        }
                    }
                }
                else if ( k == 8 )
                {
                    if ( temp[8] )
                    {
                        if ( !temp[4] )
                        {
                            temp[4] = true; temp[8] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal + lcoeff * rcoeff;
                            }
                        }
                    }
                }
                else if ( k == 9 )
                {
                    if ( temp[9] )
                    {
                        if ( !temp[5] )
                        {
                            temp[5] = true; temp[9] = false;
                            if ( dets[lindex] == temp )
                            {
                                itotal = itotal - lcoeff * rcoeff;
                            }
                        }
                    }
                }
            }
        }
    }
    std::complex<double> total(rtotal,itotal);
    return total;
}

std::string HInt (short s, short ms1, short ms2,
                    const std::vector<CSFType> & gs_csfs,
                    const std::vector<CSFType> & csfs,
                    const std::vector<std::vector<bool>> & dets )
{
    std::string result = "";
    short gs_index1 = (s-ms1)/2;
    short gs_index2 = (s-ms2)/2;
    for ( unsigned int i = 0; i < csfs.size(); i++ )
    {
        std::complex<double> temp1 = LXSX(csfs[i],gs_csfs[gs_index1],dets);
        std::complex<double> temp2 = LXSX(csfs[i],gs_csfs[gs_index2],dets);
        if ( (std::imag(temp1) > 0.01 || std::imag(temp1) < -0.01) &&
             (std::imag(temp2) > 0.01 || std::imag(temp2) < -0.01) )
        {
            result += std::to_string(-std::imag(temp2)*std::imag(temp1)) + " ΔE(0-" + std::to_string(i) + ")^(-1)\n";
        }
        temp1 = LXSX(csfs[i],gs_csfs[gs_index1],dets);
        temp2 = LYSY(csfs[i],gs_csfs[gs_index2],dets);
        if ( (std::imag(temp1) > 0.01 || std::imag(temp1) < -0.01) &&
             (std::real(temp2) > 0.01 || std::real(temp2) < -0.01) )
        {
            result += std::to_string(std::real(temp2)*std::imag(temp1)) + "I ΔE(0-" + std::to_string(i) + ")^(-1)\n";
        }
        temp1 = LXSX(csfs[i],gs_csfs[gs_index1],dets);
        temp2 = LZSZ(csfs[i],gs_csfs[gs_index2],dets);
        if ( (std::imag(temp1) > 0.01 || std::imag(temp1) < -0.01) &&
             (std::imag(temp2) > 0.01 || std::imag(temp2) < -0.01) )
        {
            result += std::to_string(-std::imag(temp2)*std::imag(temp1)) + " ΔE(0-" + std::to_string(i) + ")^(-1)\n";
        }
        temp1 = LYSY(csfs[i],gs_csfs[gs_index1],dets);
        temp2 = LXSX(csfs[i],gs_csfs[gs_index2],dets);
        if ( (std::real(temp1) > 0.01 || std::real(temp1) < -0.01) &&
             (std::imag(temp2) > 0.01 || std::imag(temp2) < -0.01) )
        {
            result += std::to_string(-std::imag(temp2)*std::real(temp1)) + " ΔE(0-" + std::to_string(i) + ")^(-1)\n";
        }
        temp1 = LYSY(csfs[i],gs_csfs[gs_index1],dets);
        temp2 = LYSY(csfs[i],gs_csfs[gs_index2],dets);
        if ( (std::real(temp1) > 0.01 || std::real(temp1) < -0.01) &&
             (std::real(temp2) > 0.01 || std::real(temp2) < -0.01) )
        {
            result += std::to_string(std::real(temp2)*std::real(temp1)) + " ΔE(0-" + std::to_string(i) + ")^(-1)\n";
        }
        temp1 = LYSY(csfs[i],gs_csfs[gs_index1],dets);
        temp2 = LZSZ(csfs[i],gs_csfs[gs_index2],dets);
        if ( (std::real(temp1) > 0.01 || std::real(temp1) < -0.01) &&
             (std::imag(temp2) > 0.01 || std::imag(temp2) < -0.01) )
        {
            result += std::to_string(-std::imag(temp2)*std::real(temp1)) + "I ΔE(0-" + std::to_string(i) + ")^(-1)\n";
        }
        temp1 = LZSZ(csfs[i],gs_csfs[gs_index1],dets);
        temp2 = LZSZ(csfs[i],gs_csfs[gs_index2],dets);
        if ( (std::imag(temp1) > 0.01 || std::imag(temp1) < -0.01) &&
             (std::imag(temp2) > 0.01 || std::imag(temp2) < -0.01) )
        {
            result += std::to_string(-std::imag(temp2)*std::imag(temp1)) + " ΔE(0-" + std::to_string(i) + ")^(-1)\n";
        }
        temp1 = LZSZ(csfs[i],gs_csfs[gs_index1],dets);
        temp2 = LYSY(csfs[i],gs_csfs[gs_index2],dets);
        if ( (std::imag(temp1) > 0.01 || std::imag(temp1) < -0.01) &&
             (std::real(temp2) > 0.01 || std::real(temp2) < -0.01) )
        {
            result += std::to_string(std::real(temp2)*std::imag(temp1)) + "I ΔE(0-" + std::to_string(i) + ")^(-1)\n";
        }
        temp1 = LZSZ(csfs[i],gs_csfs[gs_index1],dets);
        temp2 = LXSX(csfs[i],gs_csfs[gs_index2],dets);
        if ( (std::imag(temp1) > 0.01 || std::imag(temp1) < -0.01) &&
             (std::imag(temp2) > 0.01 || std::imag(temp2) < -0.01) )
        {
            result += std::to_string(-std::imag(temp2)*std::imag(temp1)) + " ΔE(0-" + std::to_string(i) + ")^(-1)\n";
        }
    }
    return result;
}

int main ()
{
    short nel, s(0);

    std::cout << "How many electrons are in your system? ";
    std::cin >> nel;

    std::vector<short> orbs(nel);
    std::vector<short> spins;

    std::cout << "\nHere are the orbital labels:\n";
    std::cout << "0: xz(alpha)\n1: xz(beta)\n2: yz(alpha)\n";
    std::cout << "3: yz(beta)\n4: xy(alpha)\n5: xy(beta)\n";
    std::cout << "6: z2(alpha)\n7: z2(beta)\n8: x2-y2(alpha)\n";
    std::cout << "9: x2-y2(beta)\n\n";
    std::cout << "What is the ground state orbital configuration in the ";
    std::cout << "Ms = S spin projection (space separated)?\n";

    for ( unsigned int i = 0; i < nel; i++ )
    {
        std::cin >> orbs[i];
        if ( orbs[i] % 2 == 0 ) s++;
        else s--;
    }

    std::cout << "S: ";
    std::cout << std::fixed << std::setprecision(1) << float (s/2.) << "\n\n";
    int min = std::min(int(nel),10-nel);
    int total_csf_count = 0;

    for ( int i = min; i >= 0; i-=2 )
    {
        if ( i < 6 )
        {
            int temp = NumCSF(i,nel);
            spins.push_back(i);
            total_csf_count += temp*(i+1);
            std::cout << "Number of CSFs (S=" << float (i/2.) << "): " << temp << "\n";
        }
    }

    int n_dets = Binomial(10,nel);

    std::cout << "\nThere are " << n_dets << " possible determinants accross all possible S.\n";

    std::vector<std::vector<bool>> dets (n_dets, std::vector<bool>(10,false));
    std::vector<std::vector<short>> configs (n_dets, std::vector<short>(5,0));
    std::vector<short> det_ms(n_dets, 0);

    GenerateDeterminants(dets,nel);
    SetDetMs(dets,det_ms,n_dets);
    SetDetConfig(dets,configs,n_dets);

    CSFType dummy;
    std::vector<CSFType> csfs(total_csf_count,dummy);

    GenerateCSFS(csfs,spins,dets,configs,det_ms,n_dets,nel);

    for ( unsigned int i = 0; i < total_csf_count; i++ )
    {
        std::cout << "CSF " << std::setw(4) << i+1 << ": ";
        int dindex;
        for ( unsigned int j = 0; j < csfs[i].count; j++ )
        {
            std::cout << std::fixed << std::setprecision(4) << GetCoeff(csfs[i],j) << " |";
            dindex = GetIndex(csfs[i],j);
            std::cout << "xz(";
            if ( dets[dindex][0] && dets[dindex][1] ) std::cout << "2)";
            else if ( dets[dindex][0] ) std::cout << "a)";
            else if ( dets[dindex][1] ) std::cout << "b)";
            else std::cout << "0)";
            std::cout << "yz(";
            if ( dets[dindex][2] && dets[dindex][3] ) std::cout << "2)";
            else if ( dets[dindex][2] ) std::cout << "a)";
            else if ( dets[dindex][3] ) std::cout << "b)";
            else std::cout << "0)";
            std::cout << "xy(";
            if ( dets[dindex][4] && dets[dindex][5] ) std::cout << "2)";
            else if ( dets[dindex][4] ) std::cout << "a)";
            else if ( dets[dindex][5] ) std::cout << "b)";
            else std::cout << "0)";
            std::cout << "z2(";
            if ( dets[dindex][6] && dets[dindex][7] ) std::cout << "2)";
            else if ( dets[dindex][6] ) std::cout << "a)";
            else if ( dets[dindex][7] ) std::cout << "b)";
            else std::cout << "0)";
            std::cout << "x2y2(";
            if ( dets[dindex][8] && dets[dindex][9] ) std::cout << "2)";
            else if ( dets[dindex][8] ) std::cout << "a)";
            else if ( dets[dindex][9] ) std::cout << "b)";
            else std::cout << "0)";
            std::cout << ">  ";
        }
        std::cout << " (S=" << csfs[i].spin/2.0 << ",MS=" << det_ms[dindex]/2. << ")\n";
    }

    std::vector<bool> gs_det_hs(10,false);
    std::vector<short> gs_config(5,0);
    std::vector<CSFType> gs_csfs(s+1);
    std::vector<int> gs_csf_indices(s+1);
    int gs_index(0);

    for ( auto j : orbs )
    {
        gs_det_hs[j] = true;
    }
    for ( unsigned int j = 0; j < 5; j++ )
    {
        if ( gs_det_hs[2*j] && gs_det_hs[2*j+1] ) gs_config[j]=2;
        else if ( gs_det_hs[2*j] || gs_det_hs[2*j+1] ) gs_config[j]=1;
        else gs_config[j]=0;
    }

    for ( unsigned int i = 0; i < total_csf_count; i++ )
    {
        if ( csfs[i].spin == s )
        {
            int tindex = GetIndex(csfs[i],0);
            if ( configs[tindex] == gs_config )
            {
                gs_csfs[gs_index] = csfs[i];
                gs_csf_indices[gs_index] = i;
                gs_index++;
            }
        }
    }

    std::cout << "\nThe Ground State CSFs are ";
    for ( unsigned int i = 0; i < gs_csf_indices.size(); i++ )
    {
        std::cout << gs_csf_indices[i]+1 << " ";
    }
    std::cout << "\n";

    std::cout << "\n\nNow evaluating the G-Tensor. For the G-Tensor, we only need to";
    std::cout << " evaluate a single Ms state, so we'll choose Ms=S:\n\n";
    std::cout << "GXX: ";
    if ( nel <= 5 ) std::cout << "-";
    std::cout << std::fixed << std::setprecision(3) << 2.0/double(s) << " ζ *\n";

    for ( unsigned int i = 0; i < total_csf_count; i++ )
    {
        double result = std::imag(LX(gs_csfs[0],csfs[i],dets));
        if ( result > 0.01 || result < -0.01 )
        {
            std::cout << std::fixed << std::setprecision(4) << result*result;
            std::cout << " E(|" << i+1 << ">)^(-1)\n";
        }        
    }

    std::cout << "\nGXY: ";
    if ( nel <= 5 ) std::cout << "-";
    std::cout << std::fixed << std::setprecision(3) << 2.0/double(s) << " ζ *\n";
    
    for ( unsigned int i = 0; i < total_csf_count; i++ )
    {
        double result1 = std::imag(LX(gs_csfs[0],csfs[i],dets));
        double result2 = std::imag(LY(gs_csfs[0],csfs[i],dets));
        if ( ( result1 > 0.01 || result1 < -0.01 ) && ( result2 > 0.01 || result2 < -0.01 ))
        {
            std::cout << std::fixed << std::setprecision(4) << result1*result2;
            std::cout << " E(|" << i+1 << ">)^(-1)\n";
        }        
    }

    std::cout << "\nGXZ: ";
    if ( nel <= 5 ) std::cout << "-";
    std::cout << std::fixed << std::setprecision(3) << 2.0/double(s) << " ζ *\n";
    
    for ( unsigned int i = 0; i < total_csf_count; i++ )
    {
        double result1 = std::imag(LX(gs_csfs[0],csfs[i],dets));
        double result2 = std::imag(LZ(gs_csfs[0],csfs[i],dets));
        if ( ( result1 > 0.01 || result1 < -0.01 ) && ( result2 > 0.01 || result2 < -0.01 ))
        {
            std::cout << std::fixed << std::setprecision(4) << result1*result2;
            std::cout << " E(|" << i+1 << ">)^(-1)\n";
        }        
    }

    std::cout << "\nGYX: ";
    if ( nel <= 5 ) std::cout << "-";
    std::cout << std::fixed << std::setprecision(3) << 2.0/double(s) << " ζ *\n";
    
    for ( unsigned int i = 0; i < total_csf_count; i++ )
    {
        double result1 = std::imag(LY(gs_csfs[0],csfs[i],dets));
        double result2 = std::imag(LX(gs_csfs[0],csfs[i],dets));
        if ( ( result1 > 0.01 || result1 < -0.01 ) && ( result2 > 0.01 || result2 < -0.01 ))
        {
            std::cout << std::fixed << std::setprecision(4) << result1*result2;
            std::cout << " E(|" << i+1 << ">)^(-1)\n";
        }        
    }

    std::cout << "GYY: ";
    if ( nel <= 5 ) std::cout << "-";
    std::cout << std::fixed << std::setprecision(3) << 2.0/double(s) << " ζ *\n";

    for ( unsigned int i = 0; i < total_csf_count; i++ )
    {
        double result = std::imag(LY(gs_csfs[0],csfs[i],dets));
        if ( result > 0.01 || result < -0.01 )
        {
            std::cout << std::fixed << std::setprecision(4) << result*result;
            std::cout << " E(|" << i+1 << ">)^(-1)\n";
        }        
    }

    std::cout << "\nGYZ: ";
    if ( nel <= 5 ) std::cout << "-";
    std::cout << std::fixed << std::setprecision(3) << 2.0/double(s) << " ζ *\n";
    
    for ( unsigned int i = 0; i < total_csf_count; i++ )
    {
        double result1 = std::imag(LY(gs_csfs[0],csfs[i],dets));
        double result2 = std::imag(LZ(gs_csfs[0],csfs[i],dets));
        if ( ( result1 > 0.01 || result1 < -0.01 ) && ( result2 > 0.01 || result2 < -0.01 ))
        {
            std::cout << std::fixed << std::setprecision(4) << result1*result2;
            std::cout << " E(|" << i+1 << ">)^(-1)\n";
        }        
    }

    std::cout << "\nGZX: ";
    if ( nel <= 5 ) std::cout << "-";
    std::cout << std::fixed << std::setprecision(3) << 2.0/double(s) << " ζ *\n";
    
    for ( unsigned int i = 0; i < total_csf_count; i++ )
    {
        double result1 = std::imag(LZ(gs_csfs[0],csfs[i],dets));
        double result2 = std::imag(LX(gs_csfs[0],csfs[i],dets));
        if ( ( result1 > 0.01 || result1 < -0.01 ) && ( result2 > 0.01 || result2 < -0.01 ))
        {
            std::cout << std::fixed << std::setprecision(4) << result1*result2;
            std::cout << " E(|" << i+1 << ">)^(-1)\n";
        }        
    }

    std::cout << "\nGZY: ";
    if ( nel <= 5 ) std::cout << "-";
    std::cout << std::fixed << std::setprecision(3) << 2.0/double(s) << " ζ *\n";
    
    for ( unsigned int i = 0; i < total_csf_count; i++ )
    {
        double result1 = std::imag(LZ(gs_csfs[0],csfs[i],dets));
        double result2 = std::imag(LY(gs_csfs[0],csfs[i],dets));
        if ( ( result1 > 0.01 || result1 < -0.01 ) && ( result2 > 0.01 || result2 < -0.01 ))
        {
            std::cout << std::fixed << std::setprecision(4) << result1*result2;
            std::cout << " E(|" << i+1 << ">)^(-1)\n";
        }        
    }

    std::cout << "GZZ: ";
    if ( nel <= 5 ) std::cout << "-";
    std::cout << std::fixed << std::setprecision(3) << 2.0/double(s) << " ζ *\n";

    for ( unsigned int i = 0; i < total_csf_count; i++ )
    {
        double result = std::imag(LZ(gs_csfs[0],csfs[i],dets));
        if ( result > 0.01 || result < -0.01 )
        {
            std::cout << std::fixed << std::setprecision(4) << result*result;
            std::cout << " E(|" << i+1 << ">)^(-1)\n";
        }        
    }

    if ( s > 1 )
    {
        std::cout << "\n\nNow let's build the Model Interaction Matrix for S=";
        std::cout << std::fixed << std::setprecision(1) << double(s/2.0) << ":\n\n";

        for ( int ms1 = s; ms1 >= -s; ms1-=2 )
        {
            for ( int ms2 = s; ms2 >= -s; ms2-=2 )
            {
                std::cout << "<" << std::fixed << std::setprecision(1) << double(s/2.0);
                std::cout << "," << std::fixed << std::setprecision(1) << double(ms2/2.0);
                std::cout << "|H(mod)|" << std::fixed << std::setprecision(1) << double(s/2.0);
                std::cout << "," << std::fixed << std::setprecision(1) << double(ms1/2.0);
                std::cout << ">:\n";
                std::cout << HModel(s,ms1,ms2) << "\n\n";
            }
        }

        std::cout << "Now let's build the General Interaction Matrix:\n\n";

        for ( int ms1 = s; ms1 >= -s; ms1-=2 )
        {
            for ( int ms2 = s; ms2 >= -s; ms2-=2 )
            {
                std::cout << "<" << std::fixed << std::setprecision(1) << double(s/2.0);
                std::cout << "," << std::fixed << std::setprecision(1) << double(ms2/2.0);
                std::cout << "|H(int)|" << std::fixed << std::setprecision(1) << double(s/2.0);
                std::cout << "," << std::fixed << std::setprecision(1) << double(ms1/2.0) << "> ";
                std::cout << std::fixed << std::setprecision(3) << 4.0/double(s*s) << " ζ^2 *\n";
                std::cout << HInt(s,ms1,ms2,gs_csfs,csfs,dets) << "\n\n";
            }
        }

    }
    return 0;
}