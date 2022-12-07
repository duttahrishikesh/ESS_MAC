/*******************************************************************************/
/* aloha_sim.c                                 */
/* 02/03/20                                                                    */
/*                                                                             */
/*******************************************************************************/

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define S_DEBUG_N
//#define DEFER_GEN
//#define DEFER_PWS_N

/*******************************************************************************/
/* User changeable constants                                                   */
/*                                                                             */
/*******************************************************************************/

#define     No_of_Nodes         (25)


#define     size                (1.0)
#define     minislots           (3)           // MS = 7
#define     actions             (3)
#define     epochs              (100000)

//#define     P_WS_P_1            (1)           /* probability P_WS when P=1  */
#define     Defer_Duration      (100)            /* in multiple pf packets */


#define     packet_duration     (1)             /* in ms */
#define     Simulation_Duration (10000)      /* in ms */

#define     EVENT_POOL_SIZE     (46900000)

#define     TRUE                (1)
#define     FALSE               (0)
#define     alpha               (0.99)
#define     beta                (0.1)
#define		gamma				(0.9)
#define     eps_max             (1.0)





//float   Q1[actions], Q2[actions], Q3[actions],Q4[actions], Q5[actions], Q6[actions],Q7[actions], Q8[actions], Q9[actions], Q10[actions],Q11[actions], Q12[actions], Q13[actions],Q14[actions], Q15[actions], Q16[actions];
float	Q[No_of_Nodes][actions];
float   rew[No_of_Nodes], small_g[No_of_Nodes], lag[No_of_Nodes], busy_n_min[No_of_Nodes], m_slot_n[No_of_Nodes], prev_slt_n[No_of_Nodes], temp_slt[No_of_Nodes], tx_period[No_of_Nodes], busy_n_min_pr[No_of_Nodes];
int		slot_n[No_of_Nodes], tx_status[No_of_Nodes], n_pkt[No_of_Nodes], prev_ac[No_of_Nodes], ac[No_of_Nodes], pr_tx_status[No_of_Nodes], status[No_of_Nodes];
int     transmit_n1[minislots], transmit_n2[minislots];
int     action[No_of_Nodes];
float   slot_as[No_of_Nodes];
float   epsilon;
int     pkt_in_frame[No_of_Nodes];

int     q_length[No_of_Nodes];




float   delta[No_of_Nodes], lr[No_of_Nodes], topology[No_of_Nodes][No_of_Nodes];



int largest(float arr[No_of_Nodes][actions], int node_id, int no_of_actions)
{
    int i;
    int index = 0;
    // Initialize maximum element
    float max = arr[node_id][0];

    // Traverse array elements from second and
    // compare every element with current max
    for (i = 1; i < no_of_actions; i++)
        if (arr[node_id][i] > max) {
            max = arr[node_id][i];
            index = i;
        }


    return index;
}


int random_action() {


    int ran_ac;

    ran_ac = (rand() % (minislots));

    return ran_ac;

}



int    collision_check(float n1, float n2, float t2, float t1) {



    int   collision = 0;

    if ((fabsf(n1 - n2) < size) || (fabsf(n1 - t2) < size) || (fabsf(n2 - t1) < size)) {
        collision = 1;

    }


    return collision;

}



int    collision_check_sync(int n1, int n2){



    int   collision = 0;

    if (n1==n2){
		collision = 1;
	}


    return collision;

}







int main()
{
    char* fn = "main";

    int     iter, iter2;

    int     p_t_s2 = 1;
    int     tmp_k = 1;
    int     tk = 1;

    int      index;










    for (int i = 0; i < No_of_Nodes; i++) {
        small_g[i] = 1.0;
        lag[i] = (float)i * 0.0;
    }




    for (int i = 0; i < No_of_Nodes; i++) {
        for (int j = 0; j < No_of_Nodes; j++) {
            topology[i][j] = 1;

            if (i == j) {
                topology[i][j] = 0;
            }
        }
    }


/*
	topology[0][7]=0;
    topology[0][8]=0;
    topology[0][12]=0;
    topology[0][11]=0;
    topology[0][14]=0;
    topology[0][13]=0;
    topology[0][16]=0;
    topology[0][17]=0;
    topology[0][18]=0;
    topology[0][19]=0;
    topology[0][22]=0;
    topology[0][23]=0;
    topology[1][8]=0;
    topology[1][9]=0;
    topology[1][12]=0;
    topology[1][10]=0;
    topology[1][13]=0;
    topology[1][14]=0;
    topology[1][15]=0;
    topology[1][17]=0;
    topology[1][18]=0;
    topology[1][19]=0;
    topology[1][23]=0;
    topology[1][24]=0;
    topology[2][5]=0;
    topology[2][9]=0;
    topology[2][10]=0;
    topology[2][11]=0;
    topology[2][13]=0;
    topology[2][14]=0;
    topology[2][15]=0;
    topology[2][16]=0;
    topology[2][18]=0;
    topology[2][19]=0;
    topology[2][20]=0;
    topology[2][24]=0;
    topology[3][6]=0;
    topology[3][5]=0;
    topology[3][10]=0;
    topology[3][11]=0;
    topology[3][12]=0;
    topology[3][14]=0;
    topology[3][15]=0;
    topology[3][16]=0;
    topology[3][17]=0;
    topology[3][19]=0;
    topology[3][20]=0;
    topology[3][21]=0;
    topology[4][6]=0;
    topology[4][7]=0;
    topology[4][10]=0;
    topology[4][11]=0;
    topology[4][12]=0;
    topology[4][13]=0;
    topology[4][15]=0;
    topology[4][16]=0;
    topology[4][17]=0;
    topology[4][18]=0;
    topology[4][19]=0;
    topology[4][21]=0;
    topology[4][22]=0;
    topology[5][2]=0;
    topology[5][3]=0;
    topology[5][12]=0;
    topology[5][13]=0;
    topology[5][16]=0;
    topology[5][17]=0;
    topology[5][18]=0;
    topology[5][19]=0;
    topology[5][21]=0;
    topology[5][22]=0;
    topology[5][23]=0;
    topology[5][24]=0;
	 topology[6][3]=0;
 	 topology[6][4]=0;
 	 topology[6][13]=0;
 	 topology[6][14]=0;
 	 topology[6][15]=0;
 	 topology[6][17]=0;
 	 topology[6][18]=0;
 	 topology[6][19]=0;
 	 topology[6][20]=0;
 	 topology[6][22]=0;
 	 topology[6][23]=0;
 	 topology[6][24]=0;
	 topology[7][0]=0;
 	 topology[7][4]=0;
 	 topology[7][10]=0;
 	 topology[7][14]=0;
 	 topology[7][15]=0;
 	 topology[7][16]=0;
 	 topology[7][18]=0;
 	 topology[7][19]=0;
 	 topology[7][20]=0;
 	 topology[7][21]=0;
 	 topology[7][23]=0;
 	 topology[7][24]=0;
 	 topology[8][0]=0;
 	 topology[8][1]=0;
 	 topology[8][10]=0;
 	 topology[8][11]=0;
 	 topology[8][15]=0;
 	 topology[8][16]=0;
 	 topology[8][17]=0;
 	 topology[8][19]=0;
 	 topology[8][20]=0;
 	 topology[8][21]=0;
 	 topology[8][22]=0;
 	 topology[8][24]=0;
	 topology[9][1]=0;
 	 topology[9][2]=0;
 	 topology[9][11]=0;
 	 topology[9][12]=0;
 	 topology[9][15]=0;
 	 topology[9][16]=0;
 	 topology[9][17]=0;
 	 topology[9][18]=0;
 	 topology[9][20]=0;
 	 topology[9][21]=0;
 	 topology[9][22]=0;
 	 topology[9][23]=0;
	 topology[10][1]=0;
 	 topology[10][2]=0;
 	 topology[10][3]=0;
 	 topology[10][4]=0;
 	 topology[10][7]=0;
 	 topology[10][8]=0;
 	 topology[10][17]=0;
 	 topology[10][18]=0;
 	 topology[10][21]=0;
 	 topology[10][22]=0;
 	 topology[10][23]=0;
 	 topology[10][24]=0;
 	 topology[11][0]=0;
 	 topology[11][2]=0;
 	 topology[11][3]=0;
 	 topology[11][4]=0;
 	 topology[11][8]=0;
 	 topology[11][9]=0;
 	 topology[11][18]=0;
 	 topology[11][19]=0;
 	 topology[11][20]=0;
 	 topology[11][22]=0;
 	 topology[11][23]=0;
 	 topology[11][24]=0;
	 topology[12][0]=0;
 	 topology[12][1]=0;
 	 topology[12][3]=0;
 	 topology[12][4]=0;
 	 topology[12][5]=0;
 	 topology[12][9]=0;
 	 topology[12][15]=0;
 	 topology[12][19]=0;
 	 topology[12][20]=0;
 	 topology[12][21]=0;
 	 topology[12][23]=0;
 	 topology[12][24]=0;
	 topology[13][0]=0;
 	 topology[13][1]=0;
 	 topology[13][2]=0;
 	 topology[13][4]=0;
 	 topology[13][5]=0;
 	 topology[13][6]=0;
 	 topology[13][15]=0;
 	 topology[13][16]=0;
 	 topology[13][20]=0;
 	 topology[13][21]=0;
 	 topology[13][22]=0;
 	 topology[13][24]=0;
	 topology[14][0]=0;
 	 topology[14][1]=0;
 	 topology[14][2]=0;
 	 topology[14][3]=0;
 	 topology[14][6]=0;
 	 topology[14][7]=0;
 	 topology[14][16]=0;
 	 topology[14][17]=0;
 	 topology[14][20]=0;
 	 topology[14][21]=0;
 	 topology[14][22]=0;
 	 topology[14][23]=0;
	 topology[15][1]=0;
 	 topology[15][2]=0;
 	 topology[15][3]=0;
 	 topology[15][4]=0;
 	 topology[15][6]=0;
 	 topology[15][7]=0;
 	 topology[15][8]=0;
 	 topology[15][9]=0;
 	 topology[15][12]=0;
 	 topology[15][13]=0;
 	 topology[15][22]=0;
 	 topology[15][23]=0;
 	 topology[16][0]=0;
 	 topology[16][2]=0;
 	 topology[16][3]=0;
 	 topology[16][4]=0;
 	 topology[16][5]=0;
 	 topology[16][7]=0;
 	 topology[16][8]=0;
 	 topology[16][9]=0;
 	 topology[16][13]=0;
 	 topology[16][14]=0;
 	 topology[16][23]=0;
 	 topology[16][24]=0;
	 topology[17][0]=0;
 	 topology[17][1]=0;
 	 topology[17][3]=0;
 	 topology[17][4]=0;
 	 topology[17][5]=0;
 	 topology[17][6]=0;
 	 topology[17][8]=0;
 	 topology[17][9]=0;
 	 topology[17][10]=0;
 	 topology[17][14]=0;
 	 topology[17][20]=0;
 	 topology[17][24]=0;
	 topology[18][0]=0;
 	 topology[18][1]=0;
 	 topology[18][2]=0;
 	 topology[18][4]=0;
 	 topology[18][5]=0;
 	 topology[18][6]=0;
 	 topology[18][7]=0;
 	 topology[18][9]=0;
 	 topology[18][10]=0;
 	 topology[18][11]=0;
 	 topology[18][20]=0;
 	 topology[18][21]=0;
 	 topology[19][0]=0;
 	 topology[19][1]=0;
 	 topology[19][2]=0;
 	 topology[19][3]=0;
 	 topology[19][5]=0;
 	 topology[19][6]=0;
 	 topology[19][7]=0;
 	 topology[19][8]=0;
 	 topology[19][11]=0;
 	 topology[19][12]=0;
 	 topology[19][21]=0;
 	 topology[19][22]=0;
	 topology[20][2]=0;
 	 topology[20][3]=0;
 	 topology[20][6]=0;
 	 topology[20][7]=0;
 	 topology[20][8]=0;
 	 topology[20][9]=0;
 	 topology[20][11]=0;
 	 topology[20][12]=0;
 	 topology[20][13]=0;
 	 topology[20][14]=0;
 	 topology[20][17]=0;
 	 topology[20][18]=0;
	 topology[21][3]=0;
 	 topology[21][4]=0;
 	 topology[21][5]=0;
 	 topology[21][7]=0;
 	 topology[21][8]=0;
 	 topology[21][9]=0;
 	 topology[21][10]=0;
 	 topology[21][12]=0;
 	 topology[21][13]=0;
 	 topology[21][14]=0;
 	 topology[21][18]=0;
 	 topology[21][19]=0;
	 topology[22][0]=0;
 	 topology[22][4]=0;
 	 topology[22][5]=0;
 	 topology[22][6]=0;
 	 topology[22][8]=0;
 	 topology[22][9]=0;
 	 topology[22][10]=0;
 	 topology[22][11]=0;
 	 topology[22][13]=0;
 	 topology[22][14]=0;
 	 topology[22][15]=0;
 	 topology[22][19]=0;

	 topology[23][0]=0;
 	 topology[23][1]=0;
 	 topology[23][5]=0;
 	 topology[23][6]=0;
 	 topology[23][7]=0;
 	 topology[23][9]=0;
 	 topology[23][10]=0;
 	 topology[23][11]=0;
 	 topology[23][12]=0;
 	 topology[23][14]=0;
 	 topology[23][15]=0;
 	 topology[23][16]=0;
	 topology[24][1]=0;
 	 topology[24][2]=0;
 	 topology[24][5]=0;
 	 topology[24][6]=0;
 	 topology[24][7]=0;
 	 topology[24][8]=0;
 	 topology[24][10]=0;
 	 topology[24][11]=0;
 	 topology[24][12]=0;
 	 topology[24][13]=0;
 	 topology[24][16]=0;
 	 topology[24][17]=0;

*/



       for (int i = 0; i < No_of_Nodes; i++) {
            for (int j = 0; j < No_of_Nodes; j++) {
                topology[i][j] = 0;
            }
        }


        for (int i = 0; i < No_of_Nodes; i++) {
            if ((i > 1) && (i < No_of_Nodes - 2)) {
                topology[i][i - 1] = 1;
                topology[i][i - 2] = 1;
                topology[i][i + 1] = 1;
                topology[i][i + 2] = 1;
            }
        }

        
            topology[0][23]=1;
            topology[0][24]=1;
            topology[0][1]=1;
            topology[0][2]=1;
            topology[1][0]=1;
            topology[1][24]=1;
            topology[1][3]=1;
            topology[1][2]=1;
            topology[24][23]=1;
            topology[24][22]=1;
            topology[24][1]=1;
            topology[24][0]=1;
            topology[23][22]=1;
            topology[23][21]=1;
            topology[23][24]=1;
            topology[23][0]=1;


/*

        topology[0][48] = 1;
        topology[0][49] = 1;
        topology[0][1] = 1;
        topology[0][2] = 1;
        topology[1][0] = 1;
        topology[1][49] = 1;
        topology[1][3] = 1;
        topology[1][2] = 1;
        topology[49][0] = 1;
        topology[49][1] = 1;
        topology[49][48] = 1;
        topology[49][47] = 1;
        topology[48][49] = 1;
        topology[48][47] = 1;
        topology[48][46] = 1;
        topology[48][0] = 1;
    */





    /*

        topology[0][1]=1;
        topology[0][2]=1;
        topology[0][3]=1;
        topology[0][4]=1;
        topology[0][5]=1;
        topology[0][12]=1;
        topology[0][13]=1;
        topology[0][7]=1;
        topology[0][15]=1;
        topology[0][8]=1;
        topology[1][0]=1;
        topology[1][2]=1;
        topology[1][13]=1;
        topology[1][5]=1;
        topology[1][12]=1;
        topology[1][3]=1;
        topology[1][6]=1;
        topology[1][4]=1;
        topology[1][9]=1;
        topology[1][14]=1;
        topology[2][6]=1;
        topology[2][1]=1;
        topology[2][3]=1;
        topology[2][14]=1;
        topology[2][0]=1;
        topology[2][5]=1;
        topology[2][13]=1;
        topology[2][15]=1;
        topology[2][10]=1;
        topology[2][7]=1;
        topology[3][1]=1;
        topology[3][2]=1;
        topology[3][4]=1;
        topology[3][5]=1;
        topology[4][2]=1;
        topology[4][3]=1;
        topology[4][5]=1;
        topology[5][3]=1;
        topology[5][4]=1;*/






    for (int i = 0; i < No_of_Nodes; i++) {

        for (iter = 0; iter < actions; iter++) {
            Q[i][iter] = 0.0;
        }

    }


    for (iter2 = 0; iter2 < No_of_Nodes; iter2++) {
        q_length[iter2] = 0;
    }

    srand(time(NULL));



    for (int i = 0; i < No_of_Nodes; i++) {
        temp_slt[i] = -5.0;
        pr_tx_status[i] = 1;
    }











    for (int i = 0; i < No_of_Nodes; i++) {

        prev_slt_n[i] = -5.0;
        n_pkt[i] = 1;
        tx_period[i] = round(1 / small_g[i]);
    }








    FILE* fpt;

    fpt = fopen("25_node_tdma_trial_hyst_sync_tp2_2c.txt", "w");   //a: 7 ms  //b: 9 ms //c: 7 ms  //d: 8 ms


    for (int epochid = 0; epochid < epochs; epochid++) {






        for (int i = 0; i < No_of_Nodes; i++) {
            if (epochid % (int)tx_period[i] == 0) {
                n_pkt[i] = 1;
            }
            else {
                n_pkt[i] = 0;
            }
        }





        int length = 100000;

        int var = epochid % length;




        printf("\nRun ID: %d\n", epochid / length);

        if (var == 0) {
            for (int iter = 0; iter < actions; iter++) {


                for (int i = 0; i < No_of_Nodes; i++) {

                    Q[i][iter] = 0.0;
                    prev_ac[i] = random_action();
                    ac[i] = random_action();
                }





                for (int i = 0; i < No_of_Nodes; i++) {
                    lr[i] = 0.0;
                    delta[i] = 0.0;
                }




            }
        }


        epsilon = (float)eps_max * (float)exp(-((float)var / 20000.0));


        for (int j = 0; j < No_of_Nodes; j++) {

            float random1 = rand() / (double)(RAND_MAX);

            if (random1 < epsilon) {
                action[j] = random_action();
            }
            else {
                action[j] = largest(Q, j, actions);
            }

        }




        for (int i = 0; i < No_of_Nodes; i++) {

            slot_n[i] = action[i];

            if (n_pkt[i] == 0) {
                slot_n[i] = -10;
            }

        }












        float m_slt = (float)minislots - 1.00;




        for (int i = 0; i < No_of_Nodes; i++) {

            m_slot_n[i] = lag[i];
            busy_n_min[i] = (float)slot_n[i];

            float kl = (float)slot_n[i] + (float)lag[i];

            while (m_slot_n[i] <= m_slt + lag[i]) {
                if (m_slot_n[i] == kl) {
                    //transmit_n1[m_slot_n1-1]=1;
                    busy_n_min[i] = m_slot_n[i];
                }

                m_slot_n[i]++;
            }
            m_slot_n[i] = lag[i];


            if (busy_n_min[i] + size > m_slt) {
                prev_slt_n[i] = busy_n_min[i] - m_slt - 1.0;
            }
            else {
                prev_slt_n[i] = -15.0;
            }

        }








        for (int i = 0; i < No_of_Nodes; i++) {

            pr_tx_status[i] = tx_status[i];

            if (slot_n[i] != -10) {


                for (int j = 0; j < No_of_Nodes; j++) {
                    if (i != j) {

                        if (topology[i][j] == 1) {
                            status[j] = collision_check_sync(slot_n[i], slot_n[j]);
                        }
                        else {
                            status[j] = 0;
                        }
                    }
                }



                for (int k = 0; k < No_of_Nodes; k++) {

                    if (status[k] == 1) {
                        tx_status[i] = 0;
                        break;
                    }
                    else {
                        tx_status[i] = 1;
                    }

                }



            }



        }




        for (int i = 0; i < No_of_Nodes; i++) {

            if (slot_n[i] == -10) {
                tx_status[i] = -1;
                printf("No Packet \t");
            }

        }




        p_t_s2 = tmp_k;
        tmp_k = 1;



        for (int i = 0; i < No_of_Nodes; i++) {

            if (pr_tx_status[i] == -1) {
                rew[i] = 0.0;
            }
            if (pr_tx_status[i] == 0) {
                rew[i] = -1.0;
            }
            if (pr_tx_status[i] == 1) {
                rew[i] = 1.0;
            }

        }








        for (int j = 0; j < No_of_Nodes; j++) {
            if (q_length[j] < 0) {
                q_length[j] = 0;
            }
        }





        for (int i = 0; i < No_of_Nodes; i++) {

            prev_ac[i] = ac[i];
            ac[i] = action[i];
        }


        for (int i = 0; i < No_of_Nodes; i++) {

            delta[i] = rew[i] - Q[i][prev_ac[i]];

        }




        for (int iter3 = 0; iter3 < No_of_Nodes; iter3++) {
            if (delta[iter3] > 0) {
                lr[iter3] = alpha;
            }
            else {
                lr[iter3] = beta;
            }
        }


        for (int i = 0; i < No_of_Nodes; i++) {

            Q[i][prev_ac[i]] = Q[i][prev_ac[i]] + lr[i] * (rew[i] - Q[i][prev_ac[i]]);

        }


        for (int i = 0; i < No_of_Nodes; i++) {

            temp_slt[i] = prev_slt_n[i];

            if ((slot_n[i] != 10)) {
                busy_n_min_pr[i] = busy_n_min[i];
            }
        }








        for (int i = 0; i < No_of_Nodes; i++) {

            printf("Mini-slot Node %d: %d\t", i + 1, slot_n[i]);
            printf("Reward %d: %f\t", i + 1, rew[i]);
            fprintf(fpt, "%d\t", slot_n[i]);
        }

        printf("\n");

        for (int i = 0; i < No_of_Nodes; i++) {

            printf("%f\t", busy_n_min[i]);
        }

        for (int i = 0; i < No_of_Nodes; i++) {

            fprintf(fpt, "%f\t", rew[i]);
        }

        printf("\n");
        fprintf(fpt, "\n");

    }

    fclose(fpt);


}
