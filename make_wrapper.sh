#!/bin/bash

handle_build_app() {

    echo -e "Note: The container will be started in attached mode,
We need to run below given command in attached container terminal to create build,
and after that enter exit to exit attached container.\n
Command to execute in container: BOLOS_SDK=\$NANOS_SDK make\n"

    sudo docker run --rm -ti -v "$(realpath .):/app" --user "$(id -u $USER)":"$(id -g $USER)" \
        ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest
}

handle_start_emulator() {
    speculos "$(pwd)"/bin/app.elf --model nanos --display headless --seed "shoot"
}

handle_load_app() {
    echo -e "Note: The container will be started in attached mode,
We need to run below given command in attached container terminal to load an app in hardware wallet,
and after that enter exit to exit attached container.\n
Command to execute in container: BOLOS_SDK=\$NANOS_SDK make load\n"
    # We need to add the appropriate udev rules
    wget -q -O - https://raw.githubusercontent.com/LedgerHQ/udev-rules/master/add_udev_rules.sh | sudo bash
    sudo docker run --rm -ti -v "/dev/bus/usb:/dev/bus/usb" -v "$(realpath .):/app" --privileged \
        ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest
}

handle_delete_app() {
    echo -e "Note: The container will be started in attached mode,
We need to run below given command in attached container terminal to delete an app from hardware wallet,
and after that enter exit to exit attached container.\n
Command to execute in container: BOLOS_SDK=\$NANOS_SDK make delete\n"
    sudo docker run --rm -ti -v "/dev/bus/usb:/dev/bus/usb" -v "$(realpath .):/app" --privileged \
        ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest
}

handle_run_tests() {
    project_root_dir=$(pwd)
    export PYTHONPATH=$project_root_dir/tests

    echo "Executing app unit testcases"
    cd unit-tests || exit 1
    cmake -Bbuild -H. && make -C build && CTEST_OUTPUT_ON_FAILURE=1 make -C build test

    echo "Executing speculos based function testcases"
    cd ../tests/speculos || exit 1
    pip install --extra-index-url https://test.pypi.org/simple/ -r requirements.txt
    pytest -v

    echo "Executing ledgercomm based function testcases with speculos"
    cd ../ledgercomm || exit 1
    pip install -r requirements.txt
    speculos "$project_root_dir"/bin/app.elf --button-port 42000 --display headless &
    echo "Waiting for 10 5 seconds to start speculos"
    sleep 5
    speculos_process_id=$!
    pytest --headless -v
    kill $speculos_process_id
    cd "$project_root_dir" || exit 1
}

handle_choice() {
    case $1 in
    1) handle_build_app ;;
    2) handle_start_emulator ;;
    3) handle_load_app ;;
    4) handle_delete_app ;;
    5) handle_run_tests ;;
    *)
        echo "Invalid choice. Please use a valid option (1-5)."
        ;;
    esac
}

show_menu() {
    echo "========== MENU =========="
    echo "1. Build App for NanoS"
    echo "2. Start Emulator"
    echo "3. Load app in NanoS wallet"
    echo "4. Delete app from NanoS wallet"
    echo "5. Run tests"
    echo "=========================="
}

if [ $# -eq 0 ]; then
    echo "No option provided!"
    show_menu
    echo "Usage: $0 <option>"
    echo "Example: $0 1 (to display date and time)"
    exit 1
fi

handle_choice $1
