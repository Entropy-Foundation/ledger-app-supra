from nacl.signing import VerifyKey
from nacl.exceptions import BadSignatureError
from aptos_client.exception import *


def test_sign_raw_tx(cmd, model):
    message = bytes.fromhex(
        "88d16ed1aba6d77d521aafada6d6e92d7a174527adb457adfa66a9c5a0f1fe377a3d3c388310009da3820335241ba0f8b3dd7a5a4afbc45417419d1232c348df000000000000000002000000000000000000000000000000000000000000000000000000000000000104636f696e087472616e73666572010700000000000000000000000000000000000000000000000000000000000000010a73757072615f636f696e095375707261436f696e000220000000000000000000000000000000000000000000000000000000000000000108102700000000000020a10700000000006400000000000000ff0fa5d4e800000006"
    )
    bip32_path: str = "m/44'/637'/1'/0'/0'"

    pub_key, chain_code = cmd.get_public_key(
        bip32_path=bip32_path, display=False
    )  # type: bytes, bytes

    pk = VerifyKey(pub_key[1:])

    der_sig = cmd.sign_raw(bip32_path=bip32_path, data=message, model=model)

    try:
        pk.verify(signature=der_sig, smessage=message)
    except BadSignatureError as exc:
        assert False, exc
