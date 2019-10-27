#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#define MAX 101

int prilis_dlhe_mesto(int pocet_znak, int povoleny_pocet);
int vela_argumentov(int pocet_argumentov);
int neznamy_argument(char *argument);
int nezname_mesto(int znak);
void zapis_do_pola(char docasne[], char vysledok[]);
void vynulovanie_retazca(int pozicia,char pole[]);
void citanie_bezarg(char docasne_pole[],char vysledne_pole[]);
void vypis_retazca(int zhoda, char vysledok[]);
void prevod_na_velke(char *argument);
void vypis_bez_duplicit(char *pole);
void usporiadanie_bezarg(int pocet_slov, char pole[]);
int rovnaky_prefix(int zhoda, char docasne[],char vysledok[],char *argv);

int main(int argc, char *argv[])
{
    int c,i=0,zhoda=0,pocet_miest=0;
    char docasne[MAX]={0};
    char vysledok[MAX]={0};
    if(argc==1)					//pracovanie s mestami v pripade ze uzivatel nezada argument
    {
        citanie_bezarg(docasne,vysledok);
        return 0;				//koniec programu
    }
    if(vela_argumentov(argc)==1 || neznamy_argument(argv[1])==1)
    {
        return 1;
    }
    prevod_na_velke(argv[1]);
    while((c=toupper(getchar()))!=EOF)		//citanie znaku pokym tam nejaky je
    {
        if(nezname_mesto(c)==1)
        {
            return 2;
        }
        if(c=='\n')				//v pripade znaku '\n' sa jedna o cele mesto, cize nastava porovnavanie s argumentom
        {
            if(++pocet_miest>42)
            {
                fprintf(stderr,"Bolo zadanych prilis vela miest!\n");
                return 3;
            }
            if(prilis_dlhe_mesto(i,(MAX-1))==1)
            {
                return 4;
            }
            if(strcmp(argv[1],docasne)==0)		//pripad ze aj argument aj mesto su identicke
            {
                zapis_do_pola(docasne,vysledok);
                zhoda++;
            }
            else if(strncmp(argv[1],docasne,strlen(argv[1]))==0)	//pripad ze argument a mesto maju identicky iba prefix
            {
                zhoda=rovnaky_prefix(zhoda,docasne,vysledok,argv[1]);
            }
            i=-1;
            vynulovanie_retazca(0,docasne);	//vynnulovanie retazca po praci s nim
        }
        docasne[i]=c;				//zapis do pola
        i++;
    }
    vypis_retazca(zhoda,vysledok);
    return 0;
}

// Chybovy vypis v pripade, ze bolo zadanych viac argumentov
int vela_argumentov(int pocet_argumentov)
{
    if(pocet_argumentov>2)
    {
        fprintf(stderr,"Bolo zadanych prilis vela argumentov!\n");
        return 1;
    }
    else return 0;
}

/* Zapisovanie iba prveho znaku jedneho pola "mesta"
do pola vysledneho*/
void citanie_bezarg(char docasne_pole[],char vysledne_pole[])
{
    int j=0,znak;
    for(int i=0;(znak=toupper(getchar()))!= EOF;i++)
    {
        docasne_pole[i]=znak;
        vysledne_pole[j]=docasne_pole[0];
        if(znak=='\n')
        {
        j++;
        i=-1;
        }
    }
    usporiadanie_bezarg(j,vysledne_pole);
}

// Usporiadanie znakov v poli podla abecedy
void usporiadanie_bezarg(int pocet_slov, char pole[])
{
    for(int i=0; i<pocet_slov;i++)
        {
            for(int k=0;k<pocet_slov-i-1;k++)
            {
                if(pole[k+1]<pole[k])
                {
                    int temp= pole[k];
                    pole[k]=pole[k+1];
                    pole[k+1]=temp;
                }
            }
        }
   vypis_bez_duplicit(pole);
}

/*Vypis pola, ktory zamedzi dvojnasobnemu vypisaniu
jedneho znaku*/
void vypis_bez_duplicit(char* pole)
{
    printf("Enable:");
    for(int i=0; i<(int)strlen(pole);i++)
        {
            if(pole[i]!=pole[i-1]&&pole[i]!='\n')
            {
                printf("%c",pole[i]);
            }
        }
	printf("\n");

}

/* Chybovy vypis v pripade, ze ako argument bol pouzity
iny znak, ako je povoleny znak z abecedy*/
int neznamy_argument(char *argument)
{
    for(int i=0;i<(int)strlen(argument);i++)
    {
        if(isalpha(argument[i]));
        else
        {
            fprintf(stderr,"Bol zadany neznamy znak v argumente!\n");
            return 1;
        }
    }
    return 0;
}

// Prevod argumentu na velke pismena
void prevod_na_velke(char *argument)
{
    int dlzka=strlen(argument);
    for(int i=0;i<dlzka;i++)
    {
        argument[i]=toupper(argument[i]);
    }
}

//Chybovy vypis na pretecenie velkosti pola
int prilis_dlhe_mesto(int pocet_znak, int povoleny_pocet)
{
    if(pocet_znak>povoleny_pocet)
    {
        fprintf(stderr,"Bolo zadane prilis dlhe mesto!\n");
        return 1;
    }
    else return 0;
}

//Chybovy vypis v pripade neznameho znaku v meste
int nezname_mesto(int znak)
{
    if(isalpha(znak)|| znak=='\n' || znak==' ')
    {
        return 0;
    }
    else
    {
        fprintf(stderr,"Bolo zadane nezname mesto!\n");
        return 1;
    }
}

//Prepisanie jedneho pola do druheho
void zapis_do_pola(char docasne[],char vysledok[])
{
    for(int n=0; n<(int)strlen(docasne);n++)
    {
        vysledok[n]=docasne[n];
    }
}

/*Praca s polom v pripade ze ma rovnaky prefix ako druhe pole.
-ak sa jedna o prvu zhodu prefixu, nakopiruje sa pole do druheho
-ak sa jedna o druhu zhodu, predchadzajuce pole sa vynuluje az
 na prvy odlisny znak, ktory sa vlozi na 0. poziciu pola
-nasledne uz pripisuje iba dalsie prve odlisne znaky
-vracia pocet zhod*/
int rovnaky_prefix(int zhoda, char docasne[],char vysledok[],char *argv)
{
    if(zhoda==0)
    {
        zapis_do_pola(docasne,vysledok);
    }
    if(zhoda==1)
    {
        vysledok[0]=vysledok[strlen(argv)];
        vynulovanie_retazca(1,vysledok);
    }
    if(zhoda>0)
    {
        vysledok[zhoda]=docasne[strlen(argv)];
    }
    return ++zhoda;
}

//Vynulovanie retazca
void vynulovanie_retazca(int pozicia,char pole[])
{
    while(pole[pozicia]!='\0')
    {
        pole[pozicia]='\0';
        pozicia++;
    }
}

//Vypis vysledneho pola podla poctu zhod
void vypis_retazca(int zhoda, char vysledok[])
{
    if(zhoda==1)
    {
        printf("Found:%s\n",vysledok);
    }
    if(zhoda>1)
    {
        usporiadanie_bezarg(zhoda,vysledok);
    }
    if(zhoda<1)
    {
        printf("Not found%s\n",vysledok);
    }
}
