
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
/**
 * usage : $CC file dstfile
 *  file is small enough,or less than 10*k
 */
#define KiB	1024
#define MiB	1024*KiB
#define MAX_LEN 10*MiB
int main(int argc,char *argv[])
{
	char *filebuf=(char*)malloc(MAX_LEN);
	assert(argc>=1);
	FILE *srcfile=fopen(argv[1],"r");
	assert(srcfile!=NULL);
	size_t bread=fread(filebuf,sizeof(char),MAX_LEN,srcfile);
	if(filebuf[bread]!=0)
	{
		printf("set last byte 0\n");
		filebuf[bread]=0;
	}
	printf("%s\n",filebuf);
	//OK, file loaded into buffer
	//start token process
	//tokens are : \n // int = ;
	const char* tokens[]={"\n","//","int","=",";","void","{","}","return","(",")"};
	const char  token_spaces=" \n\t";
	int with_spaces
	size_t	tokens_len=sizeof(tokens)/sizeof(tokens[0]);
	for(int i=0;i!=tokens_len;i++)
	{
		printf("token[%d]=%s,indexes are:",i,strcmp(tokens[i],"\n")==0?"\\n":tokens[i]);
		char *p=filebuf;
		while(p-filebuf<(ptrdiff_t)bread)
		{
			//printf("find start with %c%c%c%c\n",*p,*(p+1),*(p+2),*(p+3));
			p=strstr(p,tokens[i]);
			//printf("diff is %d\n",p-filebuf);
			//fflush(stdout);
			if(p)
			{
				printf("%d ",(int)(p-filebuf));
				p+=strlen(tokens[i]);
			}else{
				break;
			}
		};
		printf("\n");
	}

	free(filebuf);
	return 0;
}


