// gcc -g3 parse-test.c minishell.c -L. -lft -lreadline

#include "../minishell.h"
#include <assert.h>

void test_minishell(void)
{
    t_tree *tree;
    char *tests[] = {
        "ls -la",                                   // 1. Comando simple con argumentos
		"ls -l |grep .c",							// 2. Pipe simple
        "ls -l | grep .c | wc -l",                  // 3. Múltiples pipes
        "cat < input.txt",                          // 4. Redirección de entrada
       "echo hola > output.txt",                   // 5. Redirección de salida
        "echo 'texto con espacios'",                // 6. Comillas simples
        "echo \"texto con espacios\"",              // 7. Comillas dobles
        "ls    -l     -a",                          // 8. Múltiples espacios
        "ls -l > output.txt | wc -l",               // 9. Combinación de pipe y redirección
        "cat << EOF",                               // 10. Heredoc
		"cat <input.txt", 							// 11. Redirección de entrada sin espacio 
		"cat<input.txt",							// 12. Redirección de entrada sin espacio y con el comando sin espacio 
		"ls -la <infile",                           // 13. Comando simple con argumentos y redir in  
        "ls -la <infile<infile2",                   // 14. Comando simple con argumentos y dos infiles, infile esperado: infile2 
		"\"\"|<input",								// 15. comillas  dobles vacias
		"<input",									// 16. redir solo
		"ls",   									// 17. Comando simple sin argumentos
		"<input ls| wc -a",							// 18. redireccion antes que el comando 
		"echo 'comillas''dos veces'",				// 19. comillas dos veces sin espacio en medio
		"echo 'comillas''dos veces'>outfile",		// 20. comillas dos veces sin espacio en medio y luego redir
		"echo >outfile'comillas''dos veces'",		// 21. redir y luego comillas dos veces sin espacio en medio
		"echo 'comillas'>outfile'dos veces'",		// 22. comillas cerradas y luego redir y luego comillas cerradas otra vez
		"'|",										// 23.
		"|'",										// 24.
		"$VAR",
		"$VAR$VAR", 
		"prefix $VAR suffix",
		"'$VAR'", 
		"\"$VAR\"", 
		"\"$VAR'$VAR'\"",
		"$NONEXISTENT",
		"$EMPTY", 
		"$SPACE", 
		"\"$VAR'$VAR'$VAR\"'$VAR'", 
		"$VAR\t$VAR\n$VAR", 
		"$QUOTES", 
		"$LONG", 
		"$", 
		"text$", 
		"$$$$VAR", 
		"$VAR!@#$VAR", 
		"\"$VAR", 
		"$VAR<NAME", 
		"",													//DE AQUI EN ADELANTE NO ESTAN COTEJADOS
		" ls -la | grep .c | wc -l ",
		"ls -l| grep .c | wc -l ",
		"echo 'comillas simples' | cat",
		"echo \"comillas dobles\" | cat",
		"ls -l > output.txt | wc -l",
		"echo 'comillas simples' > archivo.txt | ls",
		"ls -la < input.txt | grep 'text'",
		" ls -la < input.txt | echo 'texto'",
		"echo 'texto' | grep 'palabra' | wc -l",
		"ls -la < input.txt | grep -v 'text'",
		" ls -l | cat <input.txt >output.txt",
		"ls -la < input.txt | grep 'pattern'",
		" cat <<EOF | echo 'mensaje de heredoc'",
		"cat < input.txt | echo 'mensaje'",
		"echo \"mensaje con espacio\" > archivo.txt",
		"echo 'text' | cat | echo 'final'",
		"echo 'hola mundo' | echo \"adios mundo\"",
		"echo \"mensaje | con | pipe\"",
		"ls -la < infile.txt >outfile.txt",
		"ls -la < infile | grep 'test'",
		"cat <input.txt | ls -la >outfile.txt",
		"cat < infile < anotherfile",
		"ls -la | echo 'comando final'",
		"echo 'texto' | cat > output.txt",
		" echo 'foo' > output.txt | cat",
		" ls | echo 'text' > output.txt",
		"echo 'test' > output.txt | cat",
		"echo \"text | with | pipe\" | cat",
		"ls -la | grep 'something' | wc -l",
		"echo 'var' > output.txt | echo 'another var'",
		" ls -la | cat <input | echo 'done'",
		"cat < input.txt | ls > outfile",
		"cat <<EOF echo 'text' | wc -l",
		"ls > output.txt | cat < input.txt",
		" echo 'some text' | ls -l > output.txt",
		" echo \"text with quotes\" > output.txt",
		"echo \"'some' quotes\" | ls -la > output.txt",
		"echo 'text | here' | cat",
		" cat < input.txt | ls -l | echo 'done'",
		"ls | echo 'some text' | wc -l",
		"echo \"text | with quotes\" > output.txt",
		"cat < infile | echo 'message' | ls -l",
		"ls > output.txt | grep 'pattern' | wc -l",
		"cat < input.txt | echo 'message' > output.txt",
		"echo \" $VAR \" > output.txt",
		"echo ' $VAR ' | ls -l",
		" $VAR | ls -la",
		"echo ' $VAR > output.txt'",
		"echo $VAR | ls -l",
		"echo '$VAR | ls -l'",
		"echo ' $VAR' | grep 'value'",
		"echo \"$VAR\" > output.txt",
		"ls -la | cat | echo 'done'",
		"echo $VAR <input.txt",
		"echo $VAR >output.txt | ls",
		"echo $VAR$VAR$VAR > outfile",
		"echo \"${VAR}\" | ls -la",
		"echo '$VAR$VAR' > output.txt | ls",
		" ls | echo \"test\" | wc -l",
		"ls -la | grep 'pattern' | wc -l > outfile",
		"ls | grep 'pattern' | wc -l > outfile",
		"echo \"' $VAR'\" | cat",
		"ls | echo ' test' | cat",
		"echo '$VAR|test'",
		"ls -l | grep 'pattern' | wc -l > output.txt",
		"echo ' $VAR' | cat | wc -l",
		"cat <input.txt | echo 'message' >output.txt",
		"echo 'hello world' | ls -la > output.txt",
		"echo 'test' > output.txt | echo 'done'",
		"ls | echo 'message' | echo 'done' | wc -l",
		"echo $VAR > file.txt | grep 'test'",
		" echo ' $VAR ' | grep 'pattern' > output.txt",
		"echo \"$VAR $VAR\" | grep pattern",
		"echo $VAR | grep pattern",
		"ls -l | grep 'pattern' | echo 'done'",
		"echo ' ' | ls -la | cat | grep pattern",
		"echo ' $VAR' > output.txt | grep pattern",
		"echo 'text | with quotes' >output.txt",
		"ls | echo '$VAR' | wc -l",
		"ls -la | echo '$VAR' | wc -l",
		"ls -la|grep .c|wc -l",
		"ls -l|grep .c|wc -l",
		"echo 'comillas simples'|cat",
		"echo \"comillas dobles\"|cat",
		"ls -l>output.txt|wc -l",
		"echo 'comillas simples'>archivo.txt|ls",
		"ls -la<input.txt|grep 'text'",
		"ls -la<input.txt|echo 'texto'",
		"echo 'texto'|grep 'palabra'|wc -l",
		"ls -la<input.txt|grep -v 'text'",
		"ls -l|cat<input.txt>output.txt",
		"ls -la<input.txt|grep 'pattern'",
		"cat <<EOF|echo 'mensaje de heredoc'",
		"cat<input.txt|echo 'mensaje'",
		"echo \"mensaje con espacio\">archivo.txt",
		"echo 'text'|cat|echo 'final'",
		"echo 'hola mundo'|echo \"adios mundo\"",
		"echo \"mensaje|con|pipe\"",
		"ls -la<infile.txt>outfile.txt",
		"ls -la<infile|grep 'test'",
		"cat<input.txt|ls -la>outfile.txt",
		"cat<infile<anotherfile",
		"ls -la|echo 'comando final'",
		"echo 'texto'|cat>output.txt",
		"echo 'foo'>output.txt|cat",
		"ls|echo 'text'>output.txt",
		"echo 'test'>output.txt|cat",
		"echo \"text|with|pipe\"|cat",
		"ls -la|grep 'something'|wc -l",
		"echo 'var'>output.txt|echo 'another var'",
		"ls -la|cat<input|echo 'done'",
		"cat<input.txt|ls>outfile",
		"cat <<EOF echo 'text'|wc -l",
		"ls>output.txt|cat<input.txt",
		"echo 'some text'|ls -l>output.txt",
		"echo \"text with quotes\">output.txt",
		"echo \"'some' quotes\"|ls -la>output.txt",
		"echo 'text|here'|cat",
		"cat<input.txt|ls -l|echo 'done'",
		"ls|echo 'some text'|wc -l",
		"echo \"text|with quotes\">output.txt",
		"cat<infile|echo 'message'|ls -l",
		"ls>output.txt|grep 'pattern'|wc -l",
		"cat<input.txt|echo 'message'>output.txt",
		"echo \" $VAR \">output.txt",
		"echo ' $VAR '|ls -l",
		"$VAR|ls -la",
		"echo ' $VAR>output.txt'",
		"echo $VAR|ls -l",
		"echo '$VAR|ls -l'",
		"echo ' $VAR'|grep 'value'",
		"echo \"$VAR\">output.txt",
		"ls -la|cat|echo 'done'",
		"echo $VAR<input.txt",
		"echo $VAR>output.txt|ls",
		"echo $VAR$VAR$VAR>outfile",
		"echo \"${VAR}\"|ls -la",
		"echo '$VAR$VAR'>output.txt|ls",
		"ls|echo \"test\"|wc -l",
		"ls -la|grep 'pattern'|wc -l>outfile",
		"ls|grep 'pattern'|wc -l>outfile",
		"echo \"' $VAR'\"|cat",
		"ls|echo ' test'|cat",
		"echo '$VAR|test'",
		"ls -l|grep 'pattern'|wc -l>output.txt",
		"echo ' $VAR'|cat|wc -l",
		"cat<input.txt|echo 'message'>output.txt",
		"echo 'hello world'|ls -la>output.txt",
		"echo 'test'>output.txt|echo 'done'",
		"ls|echo 'message'|echo 'done'|wc -l",
		"echo $VAR>file.txt|grep 'test'",
		"echo ' $VAR '|grep 'pattern'>output.txt",
		"echo \"$VAR $VAR\"|grep pattern",
		"echo $VAR|grep pattern",
		"ls -l|grep 'pattern'|echo 'done'",
		"echo ' '|ls -la|cat|grep pattern",
		"echo ' $VAR'>output.txt|grep pattern",
		"echo 'text|with quotes'>output.txt",
		"ls|echo '$VAR'|wc -l",
		"ls -la|echo '$VAR'|wc -l",
		"\"\'\"\"ls|\'",
		"ls |' | echo",
		"|ls|",
		"< | ls |",
		"ls<| >echo",
		"echo '|' ls",
		"| ls' |",
		"ls | | echo",
		" 'ls' |",
		"echo| ls '",
		"' | ls |",
		"' | echo | ls",
		"| | ls | |",
		"echo ' ' | ls",
		"' ls | | >",
		"ls <input | 'echo",
		"echo || ls",
		" ls | | >output",
		"| ls | | | ls",
		"ls | | ' ' | cat",
		"ls || echo | cat",
		" ls | echo '' | ls ",
		" ls ' | ls | echo",
		"echo '$VAR|'",
		" | echo ' | ls",
		" | ls | echo |",
		"echo |' | ls |",
		"ls >| echo",
		" 'echo' > ls",
		"ls > output | echo ''",
		"echo ''| | ls",
		"ls |'| | 'cat'",
		"| 'ls' | > output",
		"'ls | | wc |",
		"| ls | | echo",
		"echo ' | ls | '",
		"echo || ls | echo ' ",
		" | ls | echo | '",
		"ls | | | >echo",
		"echo | | ls |",
		"| | ls | | echo '",
		" | ls' | echo",
		"ls | echo | | |",
		"ls | '|' | 'cat'",
		"echo ' |' ls |",
		"<ls| |echo >",
		"ls > | echo ' '",
		" | ls | | |",
		"ls | echo | | ls",
		"| | | ls | echo",
		"echo |' | ls' |",
		"| | ls | echo ''",
		"echo | | ls |",
		"| 'echo' | ls",
		" ls | | | >output",
		"ls | echo ' ' | |",
		" | ls | echo '",
		"ls || echo | ls",
		" echo | ls ",
		"echo ||' ' ls",
		" | echo | ls |",
		"echo | | ls",
		" | ls |' | echo",
		"ls | ' echo ' |",
		"echo | ls || echo",
		" ' 'ls | | echo",
		" ls || | ls |",
		" ' | echo | ls",
		"ls |echo ' ' |",
		" | | | ls | echo ",
		"ls | |echo| |",
		"ls | ' ' | 'echo'",
		"ls |' | ' 'echo",
		"| ls | echo |",
		"ls | echo ' ' | |",
		"echo | ls | ||",
		"ls | ' | echo ' |",
		"echo || | ls",
		"ls | ' ' | 'echo' |",
		"echo | | ls | |",
		"ls | echo | | |",
		"echo | | ls | ' '",
		" 'ls'| echo |",
		"ls | ' ' | ls |",
		"echo | ls' | |",
		" | | echo | ls",
		"echo | | ls",
		"ls | ' ' | ' '",
		"echo || ls | |",
		"ls | | | echo |",
		"ls | 'ls' | ' '",
		" | ls | echo ''",
		"ls | | ls | echo",
		"echo | ls | | |",
		" | ls | echo |",
		"echo | ls | | ' '",
		"ls | echo | ||",
		"ls ||| echo",
		"| | | echo | ls",
		"echo '' | ls |",
        NULL
    };

    printf("\n=== Iniciando tests de Minishell ===\n       compara con: ./a.out | diff - parse-test.check\n");
    
    for (int i = 0; tests[i] != NULL; i++) {
        printf("\nTest %d: '%s'\n", i + 1, tests[i]);
        printf("-----------------------------------------------\n");
        
        tree = processline(strdup(tests[i]));  // Usamos strdup porque processline modifica la string
        if (tree == NULL) {
            printf("Error: processline devolvió NULL\n");
            continue;
        }
		char *envp[] = {NULL};  // No necesitamos envp real porque mockeamos ft_getenv
		if(expand_tree(tree, envp))
			perror("expandtree:");//esta gestion de error es muy mejorable
        
        print_tree(tree, 0);
        free_tree(tree);
        printf("\n");
    }

    printf("\n=== Tests completados ===\n");
}

int main(void)
{
    test_minishell();
    return 0;
}
