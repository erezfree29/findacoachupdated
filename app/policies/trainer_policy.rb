class TrainerPolicy < ApplicationPolicy

attr_reader :user, :post

def initialize(user, post)
    @user = current_user
    @trainer = trainer
  end

  def create?
    return true
  end

   def update?
    record.user == user
    # - record: the restaurant passed to the `authorize` method in controller
    # - user:   the `current_user` signed in with Devise.
  end


   def destroy?

    current_user.id == trainer.user_id

  end

end
