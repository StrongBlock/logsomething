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
BOB = Account()


class Test(unittest.TestCase):

    def run(self, result=None):
        super().run(result)

    @classmethod
    def setUpClass(cls):
        SCENARIO('''
        LOGSOMETHING Smart Contract Testing - deletelog
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
        create_account("BOB", MASTER)

        COMMENT('''
        Create a log of something:
        ''')
        HOST.push_action(
            "createlog",
            {
                "who": ALICE,
                "what": "This is something I want to log."
            },
            permission=[(ALICE, Permission.ACTIVE)])

    def setUp(self):
        pass

    def test_deletelog_01(self):
        try:
            HOST.push_action(
                "deletelog",
                {
                    "log_id": 2,
                    "who": ALICE
                },
                permission=[(ALICE, Permission.ACTIVE)])
        except Error as e:
            if "log does not exist" in e.message:
                COMMENT('''
                Caught trying to delete a log that does not exist.
                ''')
            else:
                COMMENT('''
                The error is different than expected.
                ''')
                raise Error(str(e))

    def test_deletelog_02(self):
        try:
            HOST.push_action(
                "deletelog",
                {
                    "log_id": 1,
                    "who": BOB
                },
                permission=[(BOB, Permission.ACTIVE)])
        except Error as e:
            if "BOB cannot delete this log" in e.message:
                COMMENT('''
                Caught trying to delete a log as the one who did not create it.
                ''')
            else:
                COMMENT('''
                The error is different than expected.
                ''')
                raise Error(str(e))

    def test_deletelog_03(self):
        try:
            HOST.push_action(
                "deletelog",
                {
                    "log_id": 1,
                    "who": ALICE
                },
                permission=[(ALICE, Permission.ACTIVE)])
        except Error as e:
            COMMENT('''
            The error is different than expected.
            ''')
            raise Error(str(e))

    def test_deletelog_04(self):
        COMMENT('''
        Verify the log was delete:
        ''')

        table_motion = HOST.table("log", HOST)

        self.assertEqual(
            len(table_motion.json["rows"]), 0,
            '''assertEqual(len(table_motion.json["rows"]), 0)''')

    def tearDown(self):
        pass

    @classmethod
    def tearDownClass(cls):
        stop()


if __name__ == "__main__":
    unittest.main()
