#!/bin/bash

validate_model_arg() {
    if [[ "$1" != "nanos" && "$1" != "nanosp" && "$1" != "nanox" ]]; then
        echo -e "Unexpected model name passed \nPassed model name: $1\nExpected model names: nanos, nanosp, nanox"
        exit 0
    fi
}

handle_build_app() {
    validate_model_arg "$1"
    sudo docker run -d --rm -ti -v "$(realpath .):/app" --user "$(id -u $USER)":"$(id -g $USER)" \
        ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest \
        /bin/bash -c "BOLOS_SDK=\$$(echo "$1" | tr '[:lower:]' '[:upper:]')_SDK make build"
}

handle_start_emulator() {
    validate_model_arg "$1"
    speculos "$(pwd)"/bin/app.elf --model "$1" --display headless --seed "shoot"
}

handle_load_app() {
    validate_model_arg "$1"
    if [[ "$1" == "nanox" ]]; then
        echo "App loading and removal is not supported in nanox"
        exit 1
    fi
    # We need to add the appropriate udev rules
    wget -q -O - https://raw.githubusercontent.com/LedgerHQ/udev-rules/master/add_udev_rules.sh | sudo bash
    sudo docker run --rm -ti -v "/dev/bus/usb:/dev/bus/usb" -v "$(realpath .):/app" --privileged \
        ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest \
        /bin/bash -c "BOLOS_SDK=\$$(echo "$1" | tr '[:lower:]' '[:upper:]')_SDK make load"
}

handle_delete_app() {
    validate_model_arg "$1"
    if [[ "$1" == "nanox" ]]; then
        echo "App loading and removal is not supported in nanox"
        exit 1
    fi
    # We need to add the appropriate udev rules
    wget -q -O - https://raw.githubusercontent.com/LedgerHQ/udev-rules/master/add_udev_rules.sh | sudo bash
    sudo docker run --rm -ti -v "/dev/bus/usb:/dev/bus/usb" -v "$(realpath .):/app" --privileged \
        ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest \
        /bin/bash -c "BOLOS_SDK=\$$(echo "$1" | tr '[:lower:]' '[:upper:]')_SDK make delete"
}

handle_run_tests() {
    validate_model_arg "$1"
    project_root_dir=$(pwd)
    export PYTHONPATH=$project_root_dir/tests

    echo "Executing app unit testcases"
    cd unit-tests || exit 1
    cmake -Bbuild -H. && make -C build && CTEST_OUTPUT_ON_FAILURE=1 make -C build test

    echo "Executing speculos based function testcases"
    cd ../tests/speculos || exit 1
    pip install --extra-index-url https://test.pypi.org/simple/ -r requirements.txt
    pytest --model "$1" -v

    echo "Executing ledgercomm based function testcases with speculos"
    cd ../ledgercomm || exit 1
    pip install -r requirements.txt
    speculos "$project_root_dir"/bin/app.elf --button-port 42000 --model "$1" --display headless &
    echo "Waiting for 10 5 seconds to start speculos"
    sleep 5
    speculos_process_id=$!
    pytest --model "$1" --headless -v
    kill $speculos_process_id
    cd "$project_root_dir" || exit 1
}

handle_choice() {
    case $1 in
    1) handle_build_app "$2" ;;
    2) handle_start_emulator "$2" ;;
    3) handle_load_app "$2" ;;
    4) handle_delete_app "$2" ;;
    5) handle_run_tests "$2" ;;
    *)
        echo "Invalid choice. Please use a valid option (1-5)."
        ;;
    esac
}

show_menu() {
    echo "========== MENU =========="
    echo "1. Build App"
    echo "2. Start Emulator"
    echo "3. Load app"
    echo "4. Delete app"
    echo "5. Run tests"
    echo "=========================="
}

if [ $# -lt 2 ]; then
    echo "Expected number of args are not provided!"
    show_menu
    echo "Usage: $0 <option> <wallet-model>"
    echo "Example: $0 1 nanos (to build app for nanos wallet)"
    exit 1
fi

handle_choice "$@"
