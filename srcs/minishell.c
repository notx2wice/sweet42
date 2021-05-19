/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ukim <ukim@42seoul.kr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/05 13:33:36 by ukim              #+#    #+#             */
/*   Updated: 2021/05/19 01:08:58 by ukim             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

t_all	g_all;

void				sighandler(int sig_num)
{
	if (sig_num == SIGINT) // cntl + c
	{	//이때 문자열 입력을 받는 버퍼를 날리고 새로 할당 한후 커런트로 이어 줘야함.
		if (g_all.child == 1)
		{
			g_all.exit_code = 130;
			write(1, "^C\n", 3);
		}
		else
		{
			g_all.exit_code = 1;
			g_all.hist_now = g_all.hist_last;
			g_all.hist_last->data.top = 0;
			write(1, "\nmini> ", 7);
		}
		return ;
	}
	if (sig_num == SIGQUIT) // cntl + back slash
	{
		if (g_all.child == 1)
		{
			g_all.exit_code = 131;
			write(1, "Quit: 3\n", 8);
		}
		return ;
	}
}

int					is_same_hist()
{
	int				idx;

	idx = 0;
	if (g_all.thist_now->data.top != g_all.hist_now->data.top)
		return (0);
	while (idx < g_all.hist_now->data.top)
	{
		if (g_all.hist_now->data.tcarr[idx] != g_all.thist_now->data.tcarr[idx])
			return (0);
		idx++;
	}
	return (1);
}

void				add_new_hist()
{
	t_hist			*temp;

	temp = make_hs_node();
	copy_hist(&g_all.hist_now, &temp);
	g_all.hist_last->prev->next = temp; //hist_start인데 왜 hist_last가 아닌것인가..
	temp->prev = g_all.hist_last->prev;
	free_t_hist(&g_all.hist_last);
	g_all.hist_last = temp;
	g_all.hist_now = g_all.hist_last;
}

int					main(int ac, char **av, char *env[])
{
	int				c;
	(void)ac;
	(void)av;
	(void)env;

	t_hist			*temp;
	t_hist			*ttemp;
	t_split_two		*now_cmd;
	g_all.hist_last = NULL;
	get_env(env, &g_all.env_first);
	signal(SIGQUIT, sighandler);
	signal(SIGINT, sighandler);
	init_all();
	print_prompt();
	// free_t_hist(&g_all.thist_start);
	copy_all_hist();
	link_thist_last_now();
	while (read(0, &c, sizeof(int)))
	{
		get_cursor_position(&g_all.tc.curcol, &g_all.tc.currow);
		if (c == EOF_KEY) // cntl + d
		{
			if (g_all.hist_now->data.top != 0)
				continue;
			write(1, "exit\n", 5);
			exit(g_all.exit_code); // 종료시 종료 인자 전달
		}
		else if (c == UP_ARROW)
		{
			if (g_all.hist_now->prev)
			{
				g_all.hist_now = g_all.hist_now->prev; // 바로 위 히스토리로 현재를 덮어씌움
				g_all.thist_now = g_all.thist_now->prev;
				clear_all_command_line()
				write(1, g_all.hist_now->data.tcarr, g_all.hist_now->data.top);
			}
		}
		else if (c == DOWN_ARROW)
		{
			if (g_all.hist_now->next)
			{
				g_all.hist_now = g_all.hist_now->next;
				g_all.thist_now = g_all.thist_now->next;
				clear_all_command_line()
				write(1, g_all.hist_now->data.tcarr, g_all.hist_now->data.top);
			}
		}
		else if (c == BACKSPACE) // 지우기 누를때
			delete_end(&g_all.tc.curcol, &g_all.tc.currow, g_all.tc.cm, g_all.tc.ce); // 지워지세요
		else if (c == NEXT_LINE) // \n 엔터 들어왔을때
		{
			if (g_all.hist_now != g_all.hist_last)//마지막 히스토리 면 저장 하고 새로 만들고 아니면 원래 마지막꺼랑 치환
			{
				if (is_same_hist()) //hist now와 thist now가 같은지
				{
					add_new_hist() // 같으면 thist는 내비둠.
				} //히스토리를 새로 만들어서 now 히스토리의 내용을 복사해 넣고 연결 리스트 위치도 수정
				else
				{
					temp = make_hs_node();
					copy_hist(&g_all.hist_now, &temp);
					copy_hist(&g_all.thist_now, &g_all.hist_now); // 다르면 thist now에 hist now를 덮어씌움
					g_all.hist_last->prev->next = temp;
					temp->prev = g_all.hist_last->prev;
					free_t_hist(&g_all.hist_last);
					g_all.hist_last = temp;
					g_all.hist_now = g_all.hist_last;
				}
			}
			else
			{ // 현재 hist now랑 마지막으로 저장했던 hist last가 같다면 아무것도 하지 않고 다시 명령줄 대기
				if (g_all.hist_now->data.top == 0) //명령줄에 아무것도 입력하지 않은 상태였다면 출력하기
				{
					write(1, "\n", 1);
					write(1, "mini> ", PROMPT_SIZE);
					continue ;
				}
			}
			if (g_all.hist_now->data.top == 0) //명령줄에 아무것도 입력하지 않은 상태였다면 출력하기
			{
				free_t_hist(&g_all.thist_start);
				copy_all_hist();
				link_thist_last_now();
				write(1, "\n", 1);
				write(1, "mini> ", PROMPT_SIZE);
				g_all.hist_now = g_all.hist_last;
				continue ;
			}
			g_all.hist_now->data.tcarr[g_all.hist_now->data.top] = '\0';
			now_cmd = parsing(g_all.hist_now->data.tcarr);
			g_all.hist_now = g_all.hist_last;
			write(1, "\n", 1);
			if (now_cmd != NULL)
				exec_command(now_cmd);
			print_prompt();
			free_t_hist(&g_all.thist_start);
			copy_all_hist();
			link_thist_last_now();
		}
		else // maybe c should have short range for printable char
		{
			char a = c
			write(1, &a, 1);
			g_all.hist_now->data.tcarr[g_all.hist_now->data.top++] = a;

		}
		c = 0;
		// c=0 을 만들어주는 이유가 있나?
	}
}
