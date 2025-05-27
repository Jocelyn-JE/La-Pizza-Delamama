/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** namedpipe
*/

#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdio>
#include <iostream>
#include <string>

#include "../src/NamedPipe.hpp"

// static void redirect_all_stdout(void) {
//     cr_redirect_stdout();
//     cr_redirect_stderr();
// }

Test(factory_tests, create_input, .init = cr_redirect_stdout) {
    NamedPipe pipe("/tmp/test_pipe");
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        cr_assert_fail();
    }
    if (pid == 0) {
        std::cout << "Child: " << pipe.getPipePath() << std::endl;
        for (int i = 0; i < 2; i++) {
            try {
                std::string data = pipe.readString();
                std::cout << "Child received: " << data << std::endl;
                pipe.writeString("Hello back from child! ");
                pipe.writeString("This is a response string. ");
                pipe.writeString("end of response string is here\n");
            } catch (const std::runtime_error &e) {
                std::cerr << "Error in child: " << e.what() << std::endl;
                break;
            }
        }
        exit(0);
    } else {
        std::cout << "Parent: " << pipe.getPipePath() << std::endl;
        for (int i = 0; i < 2; i++) {
            try {
                pipe.writeString("Hello from parent! ");
                pipe.writeString("This is a test string. ");
                pipe.writeString("end of string is here\n");
                std::string response = pipe.readString();
                std::cout << "Parent received: " << response << std::endl;
            } catch (const std::runtime_error &e) {
                std::cerr << "Error in parent: " << e.what() << std::endl;
                break;
            }
        }
        waitpid(pid, nullptr, 0);
    }
    cr_assert_stdout_eq_str(
        "Parent: /tmp/test_pipe\n"
        "Child: /tmp/test_pipe\n"
        "Child received: Hello from parent! This is a test string. end of "
        "string is here\n"
        "Parent received: Hello back from child! This is a response string. "
        "end of response string is here\n"
        "Child received: Hello from parent! This is a test string. end of "
        "string is here\n"
        "Parent received: Hello back from child! This is a response string. "
        "end of response string is here\n");
}
