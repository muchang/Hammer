#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<time.h>
#include "aiger.h"
#include "aigmutate.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>


int getRand(int n){
    return rand()%n;
}

int main(int argc,char** argv){

    //const char* filepath=argv[1];
    //printf(".");
    time_t t;
    srand((unsigned) time(&t));
    char filepath[100]="/mnt/d/softwareTesting/AIGCover/UNSAT.aig";
    aiger* aig = aiger_init();

    /*
    FILE* file=fopen(filepath,"r");
    const char* msg = aiger_read_from_file(aig,file);
    fclose(file);
    aigmutate(aig);
    file=fopen("/mnt/d/softwareTesting/AIGCover/mutateTest.aig","w");
    aiger_write_to_file(aig,0,file);
    fclose(file);
    */
    
    FILE* pFile;   //文件指针
    u32 lSize;   // 用于文件长度
    u8* buffer; // 文件缓冲区指针
    size_t result;  // 返回值是读取的内容数量
    pFile = fopen(filepath , "rb");
    if (pFile == NULL) {fputs("File error", stderr); exit(1);}    // 如果文件错误，退出1
    // obtain file size:  获得文件大小
    fseek(pFile , 0 , SEEK_END); // 指针移到文件末位
    lSize = ftell(pFile);  // 获得文件长度
    rewind(pFile);  // 函数rewind()把文件指针移到由stream(流)指定的开始处, 同时清除和流相关的错误和EOF标记
    buffer = (char*) malloc(sizeof(char) * lSize); // 分配缓冲区，按前面的 lSize
    if (buffer == NULL) {fputs("Memory error", stderr); exit(2);}  // 内存分配错误，退出2
    result = fread(buffer, 1, lSize, pFile); // 返回值是读取的内容数量
    if (result != lSize) {fputs("Reading error", stderr); exit(3);} // 返回值如果不和文件大小，读错误
    //printf("%ld %ld",strlen(buffer),lSize);
    mutateFrombuf(&buffer, &lSize);
    // terminate // 文件终止
    fclose(pFile);
    free(buffer);
    
    return 0;
}

void mutateFrombuf(u8** buf, u32* buf_len){
    aiger* aig = aiger_init();
    const char* msg = aiger_read_from_string(aig,*buf);
    
    printf("%s",msg);
    //aigmutate(aig);
    //aiger_write_to_string (aig, 0, *buf, *buf_len);
    //printf("%s",*buf);
}

void aigmutate(aiger* aig){
    if(aig->maxvar<=0) return;
    int len=getRand(aig->maxvar)%100;
    for(int count=0;count<len;count++){
        unsigned randlit=getRand(aig->maxvar);
        if(randlit < 2){
            randlit = 2;
        }
        int mutateType=getRand(5);
        switch (mutateType)
        {
        case 0:
            if(aig->num_outputs!=0){
                aig->outputs[getRand(aig->num_outputs)].lit=randlit;
                //printf("output fuzzed\n");
            }
            
            break;
        case 1:
            if(aig->num_latches!=0){
                int r=getRand(2);
                if(r==0){
                    aig->latches[getRand(aig->num_latches)].next=randlit;
                    //printf("latch next fuzzed\n");
                }
                else{
                    int index=getRand(aig->num_latches);
                    if(aig->latches[index].reset != 0){
                        aig->latches[index].reset = 0;
                        //printf("latch reset fuzzed\n");
                    }else{
                        aig->latches[index].reset = 1;
                        //printf("latch reset fuzzed\n");
                    }
                }
            }
            break;
        case 2:
            if(aig->num_bad!=0){
                aig->bad[getRand(aig->num_bad)].lit = randlit;
                //printf("bad fuzzed\n");
            }
            break;
        case 3:
            if(aig->num_constraints!=0){
                aig->constraints[getRand(aig->num_constraints)].lit = randlit;
                //printf("constraints fuzzed\n");
            }
            break;
        case 4:
            if(aig->num_ands!=0){
                int index = getRand(aig->num_ands);
                randlit=getRand(aig->ands[index].lhs);
                if(randlit!=aig->ands[index].lhs){                    
                    if(getRand(2)==0){
                        unsigned temp=aig->ands[index].rhs0;                        
                        aig->ands[index].rhs0=randlit;                         
                        if(aiger_cycle_check(aig)!=NULL){
                            aig->ands[index].rhs0 = temp;
                            //printf("rhs0 cyclic\n");
                        }else{
                            //printf("gate fuzzed\n");
                        }
                    }else{
                        unsigned temp=aig->ands[index].rhs1;
                        aig->ands[index].rhs1=randlit;
                        
                        if(aiger_cycle_check(aig)!=NULL){
                            aig->ands[index].rhs1 = temp;
                            //printf("rhs1 cyclic\n");
                        }else{
                            //printf("gate fuzzed\n");
                        }
                         
                    }
                }
            }
            break;
        default:
            //printf("5555\n");
            break;
        }
    }
}