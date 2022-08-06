
#include "oath.h"
#include <stdio.h>
#include <string.h>

#include <cstdlib>


int power_function( int x, int y );
int oath_defense( int );
int oath_campaign( int attack_dice, int defense_dice, int defense_warbands );
void oath_campaign_min_max( int attack_dice, int defense_dice, int defense_warbands, int& out_min, int& out_max );


OathCampaignResult oath( int attack_dice, int attack_warbands, int defense_dice, int defense_warbands, int attempts )
{
  int AD = attack_dice;
  int AW = attack_warbands;
  int DD = defense_dice;
  int DW = defense_warbands;

  int min;
  int max;
  oath_campaign_min_max( AD, DD, DW, min, max );

  int entryCount = (max - min) + 1;

  int* entry = (int*) malloc( entryCount * sizeof( int ) );
  memset( entry, 0, entryCount * sizeof( int ) );

  int i = 0;
  while ( i++ < attempts )
  {
    int test = oath_campaign( AD, DD, DW );

    if ( test < min || test > max )
    {
      printf( "\n\n\n--ERRROR--\n\n\n" );
      getchar();
    }

    entry[test - min]++;
  }

  if ( ONLY_RUN_SINGLE_TEST )
  {
    i = 0;
    while ( i < entryCount )
    {
      printf( "Lost %d Warbands: %d \n", i + min, entry[i] );
      i++;
    }
  }

  int failure = 0;
  int success = 0;
  float averageOnSuccess = 0.0f;
  float averageTotal = 0.0f;
  int mode = 0;
  int modeAmount = 0;

  i = 0;
  while ( i < entryCount )
  {
    if ( (i + min) > AW )
    {
      failure += entry[i];
    }
    else
    {
      success += entry[i];
      averageOnSuccess += entry[i] * (i + min);
    }

    averageTotal += entry[i] * (i + min);

    if ( modeAmount < entry[i] )
    {
      modeAmount = entry[i];
      mode = i + min;
    }

    i++;
  }

  averageTotal /= (float) attempts;
  averageOnSuccess /= (float) success;


  free( entry );

  OathCampaignResult result = {};
  result.attempts = attempts;
  result.successes = success;
  result.successRate = ((float) success / (float) attempts) * 100.f;
  result.averageTotal = averageTotal;
  result.averageOnSuccess = averageOnSuccess;
  result.lossMinimum = min;
  result.lossMaximum = max;
  result.mode = mode;
  return result;
}


void oath_campaign_min_max( int attack_dice, int defense_dice, int defense_warbands, int& out_min, int& out_max )
{
  out_min = 1 + defense_warbands - attack_dice;
  out_min = (out_min < 0) ? 0 : out_min;
  out_max = 1 + (power_function( 2, defense_dice ) + defense_warbands) - (attack_dice / 2);
  out_max = (out_max < attack_dice) ? attack_dice : out_max;
}


int oath_defense( int dices )
{
  int multiplier = 1;
  int sum = 0;
  int i = 0;

  while ( dices > i++ )
  {
    int num = rand() % 6;
    if ( num < 2 ) {}
    else if ( num < 4 )
    {
      sum += 1;
    }
    else if ( num < 5 )
    {
      sum += 2;
    }
    else
    {
      multiplier *= 2;
    }
  }

  return multiplier * sum;
}


struct AttackRollResult
{
  int rolls;
  int deaths;
};

AttackRollResult oath_attack( int dices )
{
  int deaths = 0;
  int sum = 0;
  int i = 0;
  while ( dices > i++ )
  {
    int num = rand() % 6;
    if ( num < 3 )
    {
      sum += 1;
    }
    else if ( num < 5 )
    {
      sum += 2;
    }
    else
    {
      sum += 4;
      deaths++;
    }
  }
  sum /=2;

  AttackRollResult something;
  something.rolls = sum;
  something.deaths = deaths;

  return something;
}

int oath_campaign( int attack_dice, int defense_dice, int defense_warbands )
{
  int final_deaths;
  int defense = oath_defense( defense_dice ) + defense_warbands;
  AttackRollResult attack = oath_attack( attack_dice );
  if ( defense < attack.rolls )
  {
    final_deaths = attack.deaths;
  }
  else
  {
    final_deaths = 1 + attack.deaths + (defense - attack.rolls);
  }

  return final_deaths;
}

int power_function( int x, int y )
{
  int product = 1;

  for ( int i = 0; i < y; i++ )
  {
    product = product * x;
  }
  return product;
}
