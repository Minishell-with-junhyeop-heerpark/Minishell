/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_space.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: heerpark <heerpark@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/21 22:41:50 by junhyeop          #+#    #+#             */
/*   Updated: 2024/04/05 17:33:21 by heerpark         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_putstr_fd(char *s, int fd)
{
	if (!s || fd < 0)
		return ;
	while (*s)
	{
		write(fd, s, 1);
		s++;
	}
}

// size_t	ft_strlen(const char *s)
// {
// 	size_t	n;

// 	n = 0;
// 	while (s[n] != 0)
// 		n++;
// 	return ((size_t)n);
// }

char	**freeall(char **strs)
{
	int	i;

	i = 0;
	while (strs[i])
		free(strs[i++]);
	free(strs);
	return (NULL);
}

int	strcnt(char const *s, char c)
{
	int	n;
	int	pipe_flag;

	pipe_flag = 0;
	n = 0;
	while (*s)
	{
		if (*s != ' ' && *s != c)
			pipe_flag = 0;
		if (*s == c)
		{
			if (pipe_flag == 1)
				ft_putstr_fd("bash: syntax error near unexpected token `||'", 2);
			n++;
			pipe_flag = 1;
		}
		s++;
	}
	// if (pipe_flag) 마지막에 파이프가 오는 경우!
	// 	input_cmd()
	return (n + 1);
}

char	*split_str(char const *s, char c)
{
	int		n;
	int		i;
	char	*str;

	i = 0;
	n = 0;
	while (*s == ' ')
		s++;
	while (s[n] && s[n] != c)
		n++;
	str = (char *)malloc(sizeof(char) * (n + 1));
	if (!str)
		return (NULL);
	while (i < n)
	{
		str[i] = s[i];
		i++;
	}
	str[i] = 0;
	return (str);
}

// a>b 일때는?

int	is_redir(char cmd)
{
	if (cmd == '<' || cmd == '>')
		return (1);
	return (0);
}

int	check_redir(char *cmd)
{
	int	i;

	i = 0;
	while (cmd[i])
	{
		if (is_redir(cmd[i]))
			return (1);
		i++;
	}
	return (0);
}
// >a, b>c
// redir 기준으로 한번 더 나눔
int	add_token_ext(t_token **lst, char *cmd, int *i, int *s)
{
	if (is_redir(cmd[*i]) && *i > *s)
	{
		ft_token_add(lst, token_new(ft_strndup(&cmd[*s], *i - *s), 0)); // rediction 이전까지
		*s = *i;
	}
	if (is_redir(cmd[*i]) && *i <= *s)
	{
		while (is_redir(cmd[*i]))									// redir 모두 넘김
			*i += 1;
		ft_token_add(lst, token_new(ft_strndup(&cmd[*s], *i - *s), 1)); 	// redir 끝날때까지
		*s = *i;
		return (0);
	}
	return (1);
}

void	add_token(t_token **lst, char *cmd)
{
	int		s;
	int		i;
	
	// printf(".... %s\n",cmd);
	i = 0;
	s = 0;
	if (!check_redir(cmd))
	{
		ft_token_add(lst, token_new(ft_strdup(cmd), 0));
		return ;
	}
	while (cmd[i])
	{
		if (add_token_ext(lst, cmd, &i, &s))
			i++;
	}
	if (i > s)
		ft_token_add(lst, token_new(ft_strndup(&cmd[s], i - s), 0));
}

// input : echo ls >a
// output : echo, ls, >, a

typedef struct s_split_var {
	t_token	*lst;
	char	*backup;
	int		i;
	int		flag;
	int		start;
	int		quote;
	int		dquote;
}	t_split_var;


int	s_quote_check(char c, t_split_var *flag)
{
	if (c == '\'')
	{
		flag->quote = 1;
		flag->dquote = 0;
	}
	else if (c == '\"')
	{
		flag->quote = 0;
		flag->dquote = 1;
	}
	else
		return (0);
	return (1);
}


int	s_dquote_check(char c, t_split_var *flag)
{
	if (flag->dquote == 1)
		return (1);
	if (c == '\"' && flag->dquote == 0)
	{
		flag->dquote = 1;
		return (1);
	}
	if ((c == '\"') && flag->dquote == 1)
	{
		flag->dquote = 0;
		return (0);
	}
	return (flag->dquote);
}

int	set_len(char *str, int i, char q)
{
	i++;
	while(str[i])
	{
		if (str[i] == q)
			return (i);
		i++;
	}
	return (-1);
}

// 여기가 문제!!!
void	add_token_qutoe(t_token **lst, char *cmd)
{
	ft_token_add(lst, token_new(cmd, 0));
}

void	task_dquote(char *cmd, t_split_var *flag)
{
	int	i;

	i = 0;
	while(cmd[i] && s_dquote_check(cmd[i], flag))
		i++;
	flag->i += i;
}

void	task_quote(char *cmd, t_split_var *flag)
{
	int	i;

	i = 0;
	while(cmd[i] && s_quote_check(cmd[i], flag))
		i++;
	flag->i += i;
}

char	*make_cmd(char *cmd, t_split_var *v, char q)
{
	char	*p_cmd;
	int	i;
	int	s;
	int	ind;
	
	s = v->start;
	i = set_len(cmd, v->i, q);
	// printf("\ns, i : %d %d\n ",s, i);
	if (i == -1)
		error_msg(2);
	if (i - s <= 1)
	{
		v->i = i + 1;
		v->start = i + 1;
		return (NULL);
	}
	ind = 0;
	p_cmd = (char *)malloc(sizeof(char) * (i - s - 2) + 1);
	if (!p_cmd)
		error_msg(1);
	while (s < i)
	{
		if (cmd[s] != q)
			p_cmd[ind++] = cmd[s];
		s++;
	}
	p_cmd[ind] = 0;
	// printf("\nstr: %s\n", p_cmd);
	v->i = i + 1;
	v->start = i + 1;
	return (p_cmd);
}

t_token	*split_space(char *cmd, char space)	// pipe 단위로 나눈 것 -> 공백 단위로 나눔
{
	t_split_var	v;

	v = (t_split_var){NULL, NULL, 0, 0, 0, 0, 0};
	while (v.flag == 0 && cmd[v.i]) 			// space 기준으로 나눔!
	{
		v.dquote = 0;
		v.quote = 0;
		while (cmd[v.start] == space)			// 앞에 공백 지움!
			v.start++;
		v.i = v.start;

		while (cmd[v.i] != space && cmd[v.i])	// 문자들을 넘기는데 quote 있으면 무시하고 다넘김!!!
		{
			if (s_quote_check(cmd[v.i], &v))
			{
				if (!v.backup)
					v.backup = make_cmd(cmd, &v, cmd[v.i]);
				else
					my_strjoin(&v.backup, make_cmd(cmd, &v, cmd[v.i]));
				// printf("\nbackup : %s\n", v.backup);
			}
			else
				v.i++;
			// printf("\n\nc: %c\n\n", cmd[v.i]);
		}
		if (cmd[v.i] == '\0')
			v.flag = 1;				// 마지막이라면 종료!
		cmd[v.i] = '\0';						// 분리하기 쉽게 분기지점에 널값을 넣어주었음
		if (v.i > v.start && v.backup)
			v.backup = ft_strjoin(v.backup, &cmd[v.start]);
		if (!v.backup)
			add_token(&v.lst, &cmd[v.start]);			// redir 기준으로 다시 나눔!
		else
			add_token_qutoe(&v.lst, v.backup);
		v.backup = NULL;
		v.i++;
		v.start = v.i;
		// printf("\nafter : %s\n\n", &cmd[v.start]);
	}

	t_token *tmp = v.lst;
	while (tmp)
	{
		// printf(".... %s %d\n", tmp->cmd, tmp->redir_flag);
		tmp = tmp->next;
	}
	// printf(".... |\n");
	return (v.lst);
}






// int main()
// {
// 	char **str = my_split("sdjkfl | lksadfj | sdklf");
// 	while (*str)
// 	{
// 		printf("%s\n", *str);
// 		str++;
// 	}
// }