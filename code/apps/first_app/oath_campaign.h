
#define ONLY_RUN_SINGLE_TEST 0
#define RUN_ENTIRE_TABLE 1


struct OathCampaignResult
{
  int attempts;
  int successes;
  float successRate;
  float averageTotal;
  float averageOnSuccess;
  int lossMinimum;
  int lossMaximum;
  int mode;
};

OathCampaignResult oath( int attack_dice, int attack_warbands, int defense_dice, int defense_warbands, int attempts );

#include "oath.h"
#include <stdio.h>
#include <time.h>
#include <cstdlib>
#include <cstdint>

struct TableContent
{
  int optimalAD;
  float SR;
  float AVG;
  int mode;
};

int main()
{
  int64_t timestamp = time( 0 );
  srand( timestamp );



  if ( RUN_ENTIRE_TABLE )
  {
    int DD = 0;
    int DW = 0;
    const int maxDD = 10;
    const int maxDW = 10;
    const int maxAD = 15;
    const int AW = 14;
    int calculationsLeft = maxDD * maxDW;

    TableContent* table = (TableContent*) malloc( sizeof( TableContent ) * maxDD * maxDW );

    if ( 1 )
    {

      while ( DD < maxDD )
      {
        DW = 0;
        while ( DW < maxDW )
        {
          int bestADCount = 0;
          float lowestAVG = 99999999.f;
          float SR = 0.0f;
          int mode = 0;

          int AD = 0;
          while ( AD < maxAD )
          {
            OathCampaignResult result = oath( AD, AW, DD, DW, 50000 );
            if ( result.averageTotal < lowestAVG )
            {
              lowestAVG = result.averageTotal;
              SR = result.successRate;
              mode = result.mode;
              bestADCount = AD;
            }
            AD++;
          }

          printf( "%d calculations still to do...\n", --calculationsLeft );

          int tableIndex = DD * maxDW + DW;
          table[tableIndex].optimalAD = bestADCount;
          table[tableIndex].SR = SR;
          table[tableIndex].AVG = lowestAVG;
          table[tableIndex].mode = mode;
          DW++;
        }
        DD++;
      }
    }


    //optimal roll
    printf( "AD-|" );
    DW = 0;
    while ( DW < maxDW )
    {
      printf( "--%d--|", DW++ );
    }
    printf( "-DW-||" );

    //successRate
    printf( "SR-|" );
    DW = 0;
    while ( DW < maxDW )
    {
      printf( "---%d---|", DW++ );
    }
    printf( "-DW-||" );

    //average
    printf( "AVG|" );
    DW = 0;
    while ( DW < maxDW )
    {
      printf( "---%d---|", DW++ );
    }
    printf( "-DW----->" );

    DD = 0;
    while ( DD < maxDD )
    {
      printf( "\n %d |", DD );
      DW = 0;
      while ( DW < maxDW )
      {
        printf( "  %d", table[DD * maxDW + DW].optimalAD );
        if ( table[DD * maxDW + DW].optimalAD < 10 ) printf( "  |" );
        else printf( " |" );
        DW++;
      }

      printf( "    || %d |", DD );
      DW = 0;
      while ( DW < maxDW )
      {
        if ( table[DD * maxDW + DW].SR < 99.9f )
        {
          printf( " %2.1f%% |", table[DD * maxDW + DW].SR );
        }
        else
        {
          printf( "  100%% |" );
        }
        DW++;
      }

      printf( "    || %d |", DD );
      DW = 0;
      while ( DW < maxDW )
      {
        if ( table[DD * maxDW + DW].AVG < 10.f )
        {
          printf( "  %1.2f |", table[DD * maxDW + DW].AVG );
        }
        else
        {
          printf( " %2.2f |", table[DD * maxDW + DW].AVG );
        }

        DW++;
      }

      DD++;
    }

    printf( "\n-DD--------------------------------------------------------------------DD----------------------------------------------------------------------------------------DD----------------------------------------------------------------------------------------->" );
  }
  else
  {
    int AD = 20;
    int AW = 20;
    int DD = 3;
    int DW = 3;

    if ( ONLY_RUN_SINGLE_TEST )
    {
      OathCampaignResult test = oath( 4, AW, DD, DW, 100000 );
      printf( "\nFor %d attack dice against %d defense dice and %d defense warbands:\nmin: %d\nmax: %d\n\n", AD, DD, DW, test.lossMinimum, test.lossMaximum );
      printf( "Total attempts: %d\nFailures:       %d\nSuccesses:      %d\n\nChance to win as an attacker: %3.2f%%\n\n", test.attempts, test.attempts - test.successes, test.successes, test.successRate );
      printf( "Most common amount of warbands lost: %d\n", test.mode );
      printf( "Average amount of warbands lost in total:   %3.2f\nAverage amount of warbands lost on success: %3.2f\n", test.averageTotal, test.averageOnSuccess );

      return 0;
    }

    TableContent* table = (TableContent*) malloc( sizeof( TableContent ) * (AW + 1) );

    int lowestAVGIndex = 0;
    float lowestAVG = 99999999.f;

    int i = 0;
    while ( (AW + 1) > i )
    {
      OathCampaignResult result = oath( i, AW, DD, DW, 1000000 );
      table[i].SR = result.successRate;
      table[i].AVG = result.averageTotal;
      table[i].mode = result.mode;

      if ( table[i].AVG < lowestAVG )
      {
        lowestAVG = table[i].AVG;
        lowestAVGIndex = i;
      }

      i++;
    }

    printf( "\nFor %d attack warbands against %d defense dice and %d defense warbands:\n\n", AW, DD, DW );

    printf( "    |" );
    if ( AW > 9 ) printf( " " );
    printf( " AD |  SR%%   |  AVG  | MODE |\n" );

    i = 0;
    while ( (AW + 1) > i )
    {
      printf( "    |  %d ", i );

      if ( i < 10 )
      {
        printf( " " );
      }

      if ( table[i].SR < 100.0f )
      {
        printf( "|  %3.2f ", table[i].SR );
      }
      else
      {
        printf( "| %3.2f ", table[i].SR );
      }

      printf( "|  %3.2f |   %d  |\n", table[i].AVG, table[i].mode );

      i++;
    }

    printf( "\nThis means using %d attack dice is optimal for %d defense dice and %d defense warbands!", lowestAVGIndex, DD, DW );

    free( table );
  }
  return 0;
}
