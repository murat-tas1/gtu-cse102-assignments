#include <stdio.h>

#define X 2 /*  Segment size is X . A segment is defined by X consecutive integers.*/
#define Y 3 /* This means, you are going to count Y different segments */
int main(){
    int s[Y][X];     /* Array to store the segments */
    int current[X];  /*Current segment being processed*/
    int counts[Y] = {0}; /*Number of repetitions of these segments */
    int status,i = 0,j =0,next_num,current_segment = 0,same_segment,flag; 

    /* Open input and output files */
    FILE *fp_in = fopen("input.txt","r");
    FILE *fp_out = fopen("output.txt","w");
    if(fp_in == NULL){
        printf("input file opening error!!!\n");
        return 1;
    }
    if(fp_out == NULL){
        printf("output file opening error!!!\n");
        return 1;
    }
    /* Read the first segment from the input file */
    while(i<X){
        status = fscanf(fp_in,"%d",&current[i]);
        if(status == EOF){
            printf("no enough datas!!!\n");
            return 1;
        }
        s[0][i] = current[i]; /* Store the first segment in the array */
        ++i;
    }
    counts[0] = 1; /* Set the count for the first segment to 1 */
    ++current_segment; /* Increment the number of tracked segments */
    /* Read the rest of the numbers from the input file untill EOF */
        while(fscanf(fp_in,"%d",&next_num)!=EOF){
             /* Shift the current segment and add the new number */
            for(i=0;i<X-1;++i){
                current[i] = current[i+1];
            }
            current[i] = next_num;
            flag = 0; /* Initialize flag to check if the segment matches */
             /* Compare the current segment with stored segments */
            if(current_segment<Y){
               for(i=0;i<current_segment;++i){
                  same_segment = 1; /* Assume the segments match */
                for(j=0;j<X;++j){
                    if(s[i][j] != current[j]){
                    same_segment = 0; /*not equal break the inner loop*/
                    break;
                    }
                }
                if(same_segment == 1){
                    ++counts[i]; /* Increment the count if matched */
                    flag = 1;    /*No addition to segment*/
                }
               }
            }
            else{
                 /* If the maximum number of segments is reached, compare with all */
                for(i=0;i<Y;++i){
                    same_segment = 1;
                 for(j=0;j<X;++j){
                    if(s[i][j] != current[j]){
                    same_segment = 0;
                    break;
                    }
                }
                if(same_segment == 1){
                ++counts[i]; /* Increment the count if matched */
                flag = 1;    /*No addition to segment*/
                }
               }

            }
               /* If no matching segment is found, add the current segment */
               if(flag == 0){
                if(current_segment<Y){
                for(i=0;i<X;++i){
                    s[current_segment][i] = current[i];
                    
                }
                ++counts[current_segment];
                for(i=0;i<X;++i){
                    current[i] = s[current_segment][i];
                }
                ++current_segment;
                }
                else{ /*all segments is full then shift stored segments and add the new one*/
                    for(i=0;i<Y-1;++i){
                        for(j=0;j<X;++j){
                            s[i][j] = s[i+1][j]; /*shift stored segments*/
                        }
                        counts[i] = counts[i+1]; /*shif counting numbers because added new segment*/
                    }
                    for(i=0;i<X;++i){
                    s[Y-1][i] = current[i]; /*add the new one to last segment*/
                }
                    counts[Y-1] = 1; /* Reset the last count because it is new segment */
                    for(i=0;i<X;++i){
                    current[i] = s[Y-1][i];
                }
                }
               }

        }
    /* Write the results to the output file */
        for(i=0;i<Y;++i){
            for(j=0;j<X;++j){
            fprintf(fp_out,"%d ",s[i][j]);
            }
            fprintf(fp_out,": %d\n",counts[i]);
        }
        /* Close the files */
        fclose(fp_in);
        fclose(fp_out);
}