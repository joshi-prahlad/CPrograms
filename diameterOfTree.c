/*
 * =====================================================================================
 *
 *       Filename:  diameterOfTree.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/22/12 19:54:25
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Prahlad K. Joshi (mn), 
 *        Company:  
 *
 * =====================================================================================
 */


int diameterOfTreeRecursive(node *root,int *depth)
{
		if(!root)
		{
				*depth = -1;
				return 0;
		}
		int leftDepth = 0,rightDepth = 0, leftDia = 0, rightDia = 0;
		leftDia = diameterOfTreeRecursive(root->left,&leftDepth );

		rightDia = diameterOfTreeRecursive(root->right,&rightDepth );
		*depth = max(*leftDepth,*rightDepth)  + 1;
		return max(leftDia,rightDia, 1+ leftDepth+rightDepth);
}
int max(int a,int b, int c)
{
		return max2(max2(a,b),c);	
}
int max2(int a,int b)
{
		return	a>b?a:b;
}
