import unittest
import sys
import time
from eosfactory.eosf import *

verbosity([Verbosity.INFO, Verbosity.OUT])

CONTRACT_WORKSPACE = sys.path[0] + "/../"

# Actors of the test:
MASTER = MasterAccount()
HOST = Account()
ALICE = Account()


class Test(unittest.TestCase):

    def run(self, result=None):
        super().run(result)

    @classmethod
    def setUpClass(cls):
        SCENARIO('''
        LOGSOMETHING Smart Contract Testing - createlog
        ''')
        reset()
        create_master_account("MASTER")

        COMMENT('''
        Build & deploy the contract:
        ''')
        create_account("HOST", MASTER)
        smart = Contract(HOST, CONTRACT_WORKSPACE)
        smart.build(force=False)
        smart.deploy()

        COMMENT('''
        Create test accounts:
        ''')
        create_account("ALICE", MASTER)

    def setUp(self):
        pass

    def test_createlog_01(self):
        try:
            HOST.push_action(
                "createlog",
                {
                    "who": ALICE,
                    "what": "This is something I want to log."
                },
                permission=[(ALICE, Permission.ACTIVE)])
        except Error as e:
            COMMENT('''
            The error is different than expected.
            ''')
            raise Error(str(e))

    def test_createlog_02(self):
        COMMENT('''
        Verify the log was created:
        ''')

        table_motion = HOST.table("log", HOST)

        self.assertEqual(
            table_motion.json["rows"][0]["what"], 'This is something I want to log.',
            '''assertEqual(table_motion.json["rows"][0]["what"], 'This is something I want to log.')''')

    def tearDown(self):
        pass

    @classmethod
    def tearDownClass(cls):
        stop()


if __name__ == "__main__":
    unittest.main()
