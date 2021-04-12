/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ukim <ukim@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/05 13:43:30 by ukim              #+#    #+#             */
/*   Updated: 2021/04/12 18:30:26 by ukim             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
# include <fcntl.h>
# include <stdarg.h>
# include <string.h>
# include <stdio.h>
# include <dirent.h>
# include <signal.h>
# include <errno.h>
# include <curses.h>
# include <term.h>
# include <sys/ioctl.h>
# include <termios.h>
# include <termcap.h>
#include "../libft/libft.h"

# define EOF_KEY 4
# define BACKSPACE 127
# define LEFT_ARROW 4479771
# define RIGHT_ARROW 4414235
# define UP_ARROW 4283163
# define DOWN_ARROW 4348699
# define NEXT_LINE 10

typedef struct		s_minishell
{
	char			*name;
	char			*curdir;
	long int		exit;
	char			*line;
	int				quit;
	int				quit2;
	int				count;
	int				scope_p;
	int				forked;
	char			*exit_str;
	char			**env_array;
	char			**bin;
}					t_minishell;

typedef struct		s_tmp_input
{
	char			*tcarr;
	int				max_len;
	int				top;
}					t_tmp_input;

typedef struct		s_hist
{
	t_tmp_input		data;
	struct s_hist	*prev;
	struct s_hist	*next;
}					t_hist;

typedef struct		s_termcap
{
	struct termios	term;
	int				col;//max term col
	int				row;//max term row
	int				prompt_row;//the latest prompt row
	int				currow;
	int				curcol;
	char			*cm;
	char			*ce;
}					t_termcap;

typedef struct		s_all
{
	t_hist			*hist_start;
	t_hist			*hist_now;
	t_termcap		tc;
	t_minishell		minishell;
	t_tmp_input		tmp_input;
}					t_all;

extern t_all		g_all;

void		print_prompt(void);
int			putchar_tc(int tc);
int			nbr_length(int n);
void		cursor_win(void);
void		get_cursor_position(int *col, int *rows);
void		delete_end(int *col, int *row, char *cm, char *ce);
//init
void		init_all(void);
void		init_temp(void);
void		init_term(void);
#endif