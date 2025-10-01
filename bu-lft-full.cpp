#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

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

int main ()
{
    short nel, s(0);

    std::cout << "How many electrons are in your system? ";
    std::cin >> nel;

    std::vector<short> orbs(nel);
    std::vector<short> spins;
    std::vector<int> ncsf;

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

    for ( int i = min; i >= 0; i-=2 )
    {
        if ( i < 6 )
        {
            int temp = NumCSF(i,nel);
            spins.push_back(i);
            ncsf.push_back(temp);
            std::cout << "Number of CSFs (S=" << float (i/2.) << "): " << temp << "\n";
        }
    }

    int n_dets = Binomial(10,nel);

    std::cout << "\nThere are " << n_dets << " possible determinants accross all possible S.\n";

    std::vector<std::vector<bool>> dets (n_dets, std::vector<bool>(10,false));
    std::vector<std::vector<short>> configs (n_dets, std::vector<short>(5,0));
    std::vector<short> det_ms(n_dets, 0);

    GenerateDeterminants(dets,nel);

    return 0;
}