#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

#define EPS 1e-10 //presnost pri 10 desatinnych miestach
#define MAX_Uhol 1.4   //maximalny povoleny uhol
#define MIN 0   //najniysia hodnota intervalu
#define MAX_Vyska 100   //maximalna povolena Vyska
#define Vyska 1.5   //implicitna vyska

//Vypis instrukcii pri zadani --help
void vypis_instrukcii()
{
    printf("Zadajte:"
        "\n--tan A N M \n\t-pre porovnanie presnosti vypoctov"
        "\n\t-kde A znaci uhol v radianoch, N a M interval iteracii"
        "\n\t-interval sa ma pohybovat v rozmedzi 0<N<=M<14"
        "\n[-c X] -m A [B]"
        "\n\t-kde A (v radianoch) znaci uhol Alpha"
        "\n\tprogram vypocita a vypise vzdialenost meraneho objektu"
        "\n\tpre A plati interval 0<A<=1.4<Pi/2"
        "\n\t-ak je zadany argument B(v radianoch), udava uhol Beta"
        "\n\tprogram vypocita a vypise aj vysku meraneho objektu"
        "\n\tpre B plati rovnaky interval ako pre A"
        "\n\t-argument -c nastavuje vysku meracieho pristroja"
        "\n\tvyska je dana argumentom X (0<X<=100)"
        "\n\tpri nezadani argumentu je vyska nastavena na 1.5m\n"
        );
}
//Vypocitanie tangens podla taylorovho polynomu
double taylor_tan(double x, unsigned int n);

//Vypocitanie tangens podla zretazenych zlomkov
double cfrac_tan(double x, unsigned int n);

//Vypis a porovnanie hodnot tangensu
void porovnanie(double argument, int n, int m);

//Vypocet vzdialenosti objektu
double vzdialenost(double alpha, double opposite);

//Vypocet vysky objektu
double vyska(double beta, double vzdialenost);

//Chyba intervalu uhla 0 < A <= 1.4
bool chyba_intervalu_uhla(double n);

//Chyba intervalu vysky 0 < X <= 100
bool chyba_intervalu_vysky(double vyska);

//Chyba intervalu 0 < N <= M < 14
int chyba_intervalu(int n, int m)
{
    if(n>m || n<=MIN || m>=14)
    {
        fprintf(stderr,"Chyba intervalu!\n");
        return 1;
    }
    else
    {
        return 0;
    }
}

int main(int argc, char *argv[])
{
    if(argc==1)
    {
        fprintf(stderr,"Prilis malo argumentov!\n");
        return 1;
    }
    char * chybny_vstup=0;
    if(strcmp(argv[1],"--help")==0)
    {
        vypis_instrukcii();
    }
    else if(strcmp(argv[1],"--tan")==0 && argc == 5)
    {
        double argument = strtod(argv[2],&chybny_vstup);
        if(*chybny_vstup!='\0')
        {
            fprintf(stderr,"Chyba zadaneho cisla!\n");
            return 1;
        }
        int n=strtol(argv[3],NULL,10);
        int m=strtol(argv[4],NULL,10);
        if(chyba_intervalu(n,m))
        {
            return 1;
        }
        porovnanie(argument,n,m);
    }
    else if(strcmp(argv[1],"-m")==0 && argc > 2 && argc <5)
    {
        double v,d;
        double alpha=strtod(argv[2],&chybny_vstup);
        if(*chybny_vstup!='\0')
        {
            fprintf(stderr,"Chyba zadaneho cisla!\n");
            return 1;
        }
        if(chyba_intervalu_uhla(alpha))
            return 1;
        d=vzdialenost(alpha,Vyska);
        printf("%.10e\n",d);
        if(argc==4)
        {
            double beta=strtod(argv[3],&chybny_vstup);
            if(*chybny_vstup!='\0')
            {
                fprintf(stderr,"Chyba zadaneho cisla!\n");
                return 1;
            }
            if(chyba_intervalu_uhla(beta))
                return 1;
            v=Vyska+vyska(beta,d);
            printf("%.10e\n",v);
        }
    }
    else if(strcmp(argv[1],"-c")==0 && argc > 4 && argc < 7)
    {
        double v,d,c;
        if(strcmp(argv[3],"-m")!=0)
        {
            fprintf(stderr,"Chyba argumentu! \n");
            return 1;
        }
        c=strtod(argv[2],&chybny_vstup);
        if(*chybny_vstup!='\0')
        {
            fprintf(stderr,"Chyba zadaneho cisla!\n");
            return 1;
        }
        if(chyba_intervalu_vysky(c))
            return 1;
        double alpha=strtod(argv[4],&chybny_vstup);
        if(*chybny_vstup!='\0')
        {
            fprintf(stderr,"Chyba zadaneho cisla!\n");
            return 1;
        }
        if(chyba_intervalu_uhla(alpha))
            return 1;
        d=vzdialenost(alpha,c);
        printf("%.10e\n",d);
        if(argc==6)
        {
            double beta=strtod(argv[5],&chybny_vstup);
            if(*chybny_vstup!='\0')
            {
                fprintf(stderr,"Chyba zadaneho cisla!\n");
                return 1;
            }
            if(chyba_intervalu_uhla(beta))
                return 1;
            v=c+vyska(beta,d);
            printf("%.10e\n",v);
        }
    }
    else
    {
        printf("Chyba argumentu !\n");
        return 1;
    }
    return 0;
}
void porovnanie(double argument, int n, int m)
{
    double taylor,cfrac,chyba,chyba_cf;
    double tangens=tan(argument);
    while (n<=m)
    {
        taylor=taylor_tan(argument,n);
        chyba=tangens-taylor;
        cfrac=cfrac_tan(argument,n);
        chyba_cf=tangens-cfrac;
        printf("%d %e %e %e %e %e\n",n,tangens,taylor,fabs(chyba),cfrac,fabs(chyba_cf));
        n++;
    }
}
double cfrac_tan(double x, unsigned int n)
{
    double cf=0.;
    double a,b=x*x;;
    for(unsigned int j=n;j>0;j--)
    {
        a=2*j-1;
        if(j==1)
        {
            cf=x/(a-cf);
        }
        else
        {
            cf=b/(a-cf);
        }
    }
    return cf;
}
double taylor_tan(double x, unsigned int n)
{

    const double pole_citatelov[]={1, 1, 2, 17, 62, 1382, 21844, 929569, 6404582, 443861162, 18888466084, 113927491862, 58870668456604};
    const double pole_menovatelov[]={1, 3, 15, 315, 2835, 155925, 6081075, 638512875, 10854718875, 1856156927625, 194896477400625, 49308808782358125, 3698160658676859375};
    double taylor=0;
    double umocnene=x;
    for(unsigned int i=0;i<n;i++)
    {
        if(i==0)
        {
            taylor=x*pole_citatelov[i]/pole_menovatelov[i];
        }
        else
        {
            umocnene=umocnene*x*x;
            taylor+=(umocnene*pole_citatelov[i]/pole_menovatelov[i]);
        }
    }
    return taylor;
}
double vzdialenost(double alpha, double opposite)
{
    int i=1;
    double x=0.0;
    double old_x;
    do
    {
        old_x=x;
        x=cfrac_tan(alpha,i);
        i++;
    }while(fabs(x-old_x)>EPS);
    double strana = opposite / x;
    return strana;
}
double vyska(double beta, double vzdialenost)
{
    int i=1;
    double x=0.0;
    double old_x;
    do
    {
        old_x=x;
        x=cfrac_tan(beta,i);
        i++;
    }while(fabs(x-old_x)>EPS);
    double strana = x*vzdialenost;
    return strana;
}
bool chyba_intervalu_uhla(double n)
{
    if(n>MIN && n<=MAX_Uhol)
    {
        return false;
    }
    else
    {
        fprintf(stderr,"Chyba intervalu uhla !\n");
        return true;
    }
}
bool chyba_intervalu_vysky(double vyska)
{
    if(MIN<vyska && vyska<=MAX_Vyska)
    {
        return false;
    }
    fprintf(stderr,"Nespravny interval vysky !\n");
    return true;
}
