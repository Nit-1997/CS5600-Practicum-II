#!/bin/bash

SERVER_IP="127.0.0.1"
SERVER_PORT="8081"
CLIENT_BINARY="client-code/rfs"
SERVER_FILE_SYSTEM="server-code/server-file-system"
CLIENT_FILE_SYSTEM="client-code/client-file-system"
v1="_v1"  

function test_write() {
     echo "******************* Test 1 ***************************"
    local local_file="$1"
    local remote_file="$2"
    
    # Perform WRITE operation
    
    $CLIENT_BINARY WRITE "$local_file" "$remote_file"
    
    # Compare contents of local and remote files
    local_file_path="$CLIENT_FILE_SYSTEM/$local_file"
    remote_file_path="$SERVER_FILE_SYSTEM/$remote_file$v1"
    
    if cmp -s "$local_file_path" "$remote_file_path"; then
        echo "******************* Test Passed ***************************"
        echo "Contents of $local_file and $remote_file are the same."
    else
        echo "******************* Test Failed ***************************"
        echo "Contents of $local_file and $remote_file are different."
    fi

    echo "******************* Clean up using RM ***************************"

    $CLIENT_BINARY RM "$remote_file"
}

function test_get() {
    echo "******************* Test 2 ***************************"
    $CLIENT_BINARY WRITE "test1.txt" "blank.txt"
    $CLIENT_BINARY WRITE "test2.txt" "blank.txt"
    $CLIENT_BINARY WRITE "test3.txt" "blank.txt"

    $CLIENT_BINARY GET "blank.txt" "1"
    if cmp -s "client-file-system/blank.txt" "client-file-system/test1.txt"; then
        echo "******************* Test Passed ***************************"
    else
        echo "******************* Test Failed ***************************"
    fi

    $CLIENT_BINARY GET "blank.txt"
    if cmp -s "client-file-system/blank.txt" "client-file-system/test3.txt"; then
        echo "******************* Test Passed ***************************"
    else
        echo "******************* Test Failed ***************************"
    fi

    $CLIENT_BINARY GET "blank.txt" "blank.txt" "1"
    if cmp -s "client-file-system/blank.txt" "client-file-system/test1.txt"; then
        echo "******************* Test Passed ***************************"
    else
        echo "******************* Test Failed ***************************"
    fi

   $CLIENT_BINARY GET "blank.txt" "blank.txt" 
    if cmp -s "client-file-system/blank.txt" "client-file-system/test3.txt"; then
        echo "******************* Test Passed ***************************"
    else
        echo "******************* Test Failed ***************************"
    fi

    echo "******************* Clean up using RM ***************************"

    $CLIENT_BINARY RM "blank.txt"  
    rm "client-file-system/blank.txt"   
}

function test_ls() {
    echo "******************* Test 3 ***************************"
    $CLIENT_BINARY WRITE "test1.txt" "blank.txt"
    $CLIENT_BINARY WRITE "test2.txt" "blank.txt"
    $CLIENT_BINARY WRITE "test3.txt" "blank.txt"

    $CLIENT_BINARY LS "blank.txt" > output.txt

    if [ -s "output.txt" ]; then
        echo "******************* Test Passed ***************************"
    else
        echo "******************* Test Failed ***************************"
    fi
    
    echo "******************* Clean up using RM ***************************"

    $CLIENT_BINARY RM "blank.txt"  
    rm "output.txt"
}    


test_write "test.txt" "test.txt" # tests RM too
test_get  #tests all variations of get + RM too
test_ls # tests LS 

