/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_list.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: heerpark <heerpark@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/17 06:22:43 by junhyeop          #+#    #+#             */
/*   Updated: 2024/03/29 09:29:28 by heerpark         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_head	*init_head(char **envp)
{
	t_head	*head;

	head = (t_head *)malloc(sizeof(t_head));
	head->data = (t_data *)malloc(sizeof(t_data));
	if (!head)
		error_msg(1);
	head->top = NULL;
	head->size = 0;
	head->data->env = (t_list *)malloc(sizeof(t_list));
	set_env(&(head->data->env), envp);
	return (head);
}

// t_token	*init_token()
// {
// 	t_token	*new;

// 	new = (t_token *)malloc(sizeof(t_head));
// 	if (!head)
// 		error_msg(1);
// 	head->top = NULL;
// 	head->size = 0;
// 	return (head);
// }


t_list	*cmd_list_new(char *command)
{
	t_list	*newnode;

	newnode = (t_list *)malloc(sizeof(t_list));
	if (!newnode)
		error_msg(1);
	newnode->next = NULL;
	newnode->prev = NULL;
	newnode->token = make_token(command);
	return (newnode);
}

t_token	*token_new(char *command, int flag)
{
	t_token	*new;

	new = (t_token *)malloc(sizeof(t_token));
	if (!new)
		error_msg(1);
	new->prev = NULL;
	new->next = NULL;
	new->cmd = command;
	new->redir_flag = flag;
	return (new);
}

t_token	*make_token(char *command)
{
	// int		i;
	t_token	*token;
	
	// i = 0;
	token = split_space(command, ' ');
	return (token);
}

void	ft_token_add(t_token **lst, t_token *new)
{
	t_token	*tmp;

	tmp = *lst;
	if (tmp == NULL)
	{
		*lst = new;
		return ; 
	}
	if (tmp == NULL && new == NULL)
		error_msg(1) ;
	else
	{
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new;
		new->prev = tmp;
	}
}


void	ft_lst_add(t_head *head, t_list *new)
{
	t_list	*tmp;

	head->size += 1;
	tmp = head->top;
	if (tmp == NULL)
	{
		head->top = new;
		return ; 
	}
	else if (tmp == NULL && new == NULL)
		return ;
	else
	{
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new;
		new->prev = tmp;
	}
}
