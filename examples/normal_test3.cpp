
/*
 * 测试, 函数声明,全局变量定义,变量赋值
 *
 * STATEMENT是指那些能够构成独立句子的语句,多数情况下包括一个; 或者 {}
 * SENTENCE 是指一个更小的成分, 通常不能独立成语句
 *
 */
int a=0;
int print(int a)
{
	int num=printf("%d\n",a);
	num = num -1;
	return num;
}
