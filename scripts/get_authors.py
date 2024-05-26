import argparse
import csv
import os
import shutil

import git


def get_authors_and_emails(filter_list=None):
    authors_emails = dict()

    if not shutil.which("git"):
        authors_emails["1660583890@qq.com"] = "iveswang"
        authors_emails["nononevent@outlook.com"] = "AlanusMeminius"
    else:
        repo = git.Repo(os.getcwd())
        commits = repo.iter_commits('--all')

        for commit in commits:
            author_name = commit.author.name
            author_email = commit.author.email
            # Add the author's email and name only if the email does not contain
            # any substring in the filter list
            if filter_list is None or not any(substr in author_email for substr in filter_list):
                if author_email not in authors_emails:
                    authors_emails[author_email] = author_name

    return {(name, email) for email, name in authors_emails.items()}


def write_to_csv(authors_emails, csv_filename):
    with open(csv_filename, 'w', newline='', encoding='utf-8') as csvfile:
        csv_writer = csv.writer(csvfile)
        for author_email in authors_emails:
            csv_writer.writerow(author_email)


def main():
    parser = argparse.ArgumentParser(description='Get authors and emails')
    parser.add_argument('csv_filename', help='Path to the CSV file to write')

    args = parser.parse_args()

    filter_list = ["automotive.cn", "users.noreply.github.com", "geanevaitbecke@gmail.com"]
    authors_emails = get_authors_and_emails(filter_list)
    write_to_csv(authors_emails, args.csv_filename)


if __name__ == "__main__":
    main()
