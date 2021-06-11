#include <stdio.h>
#include <limits.h>

#define TABLE_END -1
#define ROOT_NUM 3

//時刻表
static const int busToChibaTable[] = {3, 12, 19, 25, 32, 39, 44, 51, 57, TABLE_END};
static const int busToChisiroTable[] = {3, 12, 19, 25, 32, 39, 44, 51, 57, TABLE_END};
static const int busToTugaTable[] = {18, TABLE_END};
static const int monorailTable[] = {5, 15, 25, 35, 45, 55, TABLE_END};
static const int JRTable[] = {4, 17, 31, 48, TABLE_END};

//現在時刻更新
void updateTime(int *c_h, int *c_m, int *c_d, int t){
    if((*c_m + t) < 60) {
        *c_m += t;
    }
    else{
        *c_h += (*c_m + t) / 60;
        *c_m = (*c_m + t) % 60;
    }
    //24時超えチェック 0時を一日の開始時刻とする
    if(*c_h >= 24){
        *c_d += *c_h / 24;
        *c_h = *c_h % 24;
    }
}

int main(void)
{
  int i;
  int h, m; //バス停到着時間,分
  int current_d = 0; //日数
  int current_h, current_m; //現在時刻
  int min; //最短時刻差
  int rootnum; //各ルート
  int min_h, min_m, min_d; //最短時刻,日数
  int min_root; //最短ルート
  int connecttime = 1; //乗り換え時間

  //ルート名
  char rootBusName[] = "***  情報大 -バス-> 千葉  ***";
  char rootMonorailName[] = "***  情報大 -バス-> 千城台 -モノレール-> 千葉  ***";
  char rootJRName[] = "***  情報大 -バス-> 都賀 -総武線各駅-> 千葉  ***";

  //行き先
  char busChibaName[] = "バス: 情報大 -> 千葉";
  char busChisiroName[] = "バス: 情報大 -> 千城台";
  char busTugaName[] = "バス: 情報大 -> 都賀";
  char monorailName[] = "モノレール: 千城台 -> 千葉";
  char JRName[] = "JR: 都賀 -> 千葉";

  struct root{
      char *rootname; //ルート名
      struct transit *pTrans;
  };

  //配列サイズ追加
  struct transit{
      char *name; //交通機関名
      const int *schedule; //時刻表
      int time; //所要時間
      struct transit *p;
  };


  struct root way[ROOT_NUM] = {{rootBusName,NULL},
                       {rootMonorailName, NULL},
                       {rootJRName, NULL}};

  struct transit start[] = {{busChibaName, busToChibaTable, 1440, NULL},
                            {busChisiroName, busToChisiroTable, 8, NULL},
                            {busTugaName, busToTugaTable, 27, NULL}};

  struct transit connect[] = {{monorailName, monorailTable, 24, NULL},
                              {JRName, JRTable, 6, NULL}};

  struct transit *pt;

  /*ルート情報作成*/
  way[0].pTrans = &start[0];

  way[1].pTrans = &start[1];
  start[1].p = &connect[0];

  way[2].pTrans = &start[2];
  start[2].p = &connect[1];

  printf("情報大前のバス停に何時何分に到着しますか？（入力例: 16 25）");
  scanf("%d%d", &h,&m);
  if(h > 23 || m > 60){
      printf("  入力エラー: 正しく入力してください\n  [0-23]時 [0-59]分 の間の数字を入れてください\n");
      exit(1);
  }
  else{
      //各ルート計算
      min_h = 24;
      min_m = 60; //最短ルート時刻初期化
      min_d = INT_MAX;
      min_root = 0; //最短ルート初期化

      for(rootnum = 0; rootnum < ROOT_NUM; ++rootnum){
          puts("");
          //現在時刻初期化
          current_h = h;
          current_m = m;
          current_d = 0;

          printf("%s\n",way[rootnum].rootname);

          for(pt = way[rootnum].pTrans; pt != NULL; pt = pt->p){
              printf("%s\n",pt->name);
              min = 60; //最短時刻差初期化
              //最短時刻差探索
              for(i = 0; pt->schedule[i] != TABLE_END; i++){
                  if((pt->schedule[i] - current_m) >= 0 && (pt->schedule[i] - current_m) < min){
                    min = pt->schedule[i] - current_m;
                  }
              }
              if(min == 60){
                  current_m = pt->schedule[0];
                  current_h++;
              }
              else{
                  current_m += min;
              }
              printf("出発時刻:  %11d 時 %3d 分\n",current_h, current_m);

              //現在時刻更新
              updateTime(&current_h, &current_m, &current_d, pt->time);

              if(current_d > 0) printf("到着時刻:%5d日後  %d 時 %3d 分\n",current_d, current_h, current_m);
              else printf("到着時刻: %12d 時 %3d 分\n", current_h, current_m);

              //乗り変え有無チェック
              if(pt->p != NULL){
                  //乗り変え時間追加
                  updateTime(&current_h, &current_m, &current_d, connecttime);
                  if(current_d > 0) printf("乗り換え後:%3d日後 %3d 時 %3d 分\n",current_d, current_h, current_m);
                  else printf("乗り換え後: %10d 時 %3d 分\n", current_h, current_m);
              }
          }

          //最短ルート時間更新
          //日数単位が最小値よりも小さい時
          if (min_d > current_d){
              min_d = current_d;
              min_h = current_h;
              min_m = current_m;
              min_root = rootnum;
          }
          //日数単位同じ、時間単位が小さい時
          else if(min_d == current_d && min_h > current_h){
                  min_h = current_h;
                  min_m = current_m;
                  min_root = rootnum;
          }
          //日数、時間単位同じ、分単位が小さい時
          else if(min_h == current_h && min_m > current_m){
              min_m = current_m;
              min_root = rootnum;
          }
      }
  }

  printf("--- 最も早く千葉駅に着くルートは  ---\n");
  printf("%s\n",way[min_root].rootname);
  return 0;
}
