# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rrouille <rrouille@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/02/14 15:20:40 by rrouille          #+#    #+#              #
#    Updated: 2023/02/16 20:29:31 by rrouille         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

###############
##
#Arguments

NAME	= pipex

###############
##

# Folders
SRCDIR		= srcs
OBJDIR		= objs
HDRDIR		= includes/

# Colors
GRAY		= \033[0;90m
RED			= \033[0;91m
GREEN		= \033[0;92m
YELLOW		= \033[0;93m
BLUE		= \033[0;94m
MAGENTA		= \033[0;95m
CYAN		= \033[0;96m
WHITE		= \033[0;97m
BG_G		= \033[42m
ENDCOLOR	= \033[0m

#Sources

SRCS	= ${shell find ${SRCDIR} -name '*.c' ! -name '*/*.c'}

OBJS = $(patsubst $(SRCDIR)%,$(OBJDIR)%,$(SRCS:%.c=%.o))
# OBJS	= ${SRCS:${SRCDIR}%.c=${OBJDIR}%.o} # $(filter-out $(SRCDIR)%,$(SRCS:%.c=$(OBJDIR)%.o))
CFLAGS	= -Werror -Wall -Wextra -g
CC		= gcc
RM		= rm -rf
MAKE 	= make
MKDIR	= mkdir

# Progression bar
START			=		echo "${YELLOW}\nStart of program compilation\n${ENDCOLOR}"
END_COMP		=		echo "${GREEN}\nEnd of compilation${ENDCOLOR}"
S_OBJS			=		echo "${RED}Suppression des objets\n${ENDCOLOR}"
S_NAME			=		echo "${RED}Suppression du programme${ENDCOLOR}"
CHARG_LINE		=		echo "${BG_G} ${ENDCOLOR}\c"
BS_N			=		echo "\n"

# First rule
all:		${NAME}

${OBJDIR}/%.o : ${SRCDIR}/%.c lib
#			@cp mlx/mlx.h ${HDRDIR}
			@$(CC) -I ${HDRDIR} -c $< -o $@

lib :
			@if [ -d mylib ]; then \
				echo "${GREEN}Program already exists, updating it.${ENDCOLOR}"; \
			else \
				git clone https://github.com/rphlr/mylib --quiet; \
				make -C mylib; \
			fi
			@${MKDIR} -p ${OBJDIR}
			@${START}

$(NAME): $(OBJS)
			@for i in $(OBJS) mylib/objs/*/*.o; do \
			${CHARG_LINE}; \
			done; \
			echo ""; \
			$(CC) $(CFLAGS) $(OBJS) mylib/objs/*/*.o -o $(NAME)
			@${END_COMP}

# Git repo maker
git: fclean
			@git add *
			@echo "${CYAN}Added files to git !"
			@git commit -m "Auto-commit"
			@echo "${BLUE}Commited !"
			@git push
			@echo "${GREEN}All changed are now on github!${ENDCOLOR}"

clean:
			@$(S_OBJS)
			@$(RM) $(OBJDIR) mylib outfile

fclean: clean
			@$(S_NAME)
			@${RM} ${NAME}

re: fclean all

.PHONY: all clean fclean re